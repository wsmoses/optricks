/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HO_
#define FOREACHLOOP_HO_

#include "../language/statement/Statement.hpp"
#include "./Declaration.hpp"
#include "./function/E_GEN.hpp"
#include "../language/location/Location.hpp"
#include "../language/class/GeneratorClass.hpp"
#include "../operators/LocalFuncs.hpp"

class ForEachLoop : public ErrorStatement{
	public:
		E_VAR* localVariable;
		Statement* iterable;
		Statement* toLoop;
		String name;
		ForEachLoop(PositionID id, E_VAR* var, Statement* it,Statement* tL, String n="") :
			ErrorStatement(id), localVariable(var), iterable(it),toLoop(tL){
			name = n;
		}
		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		void registerClasses() const override final{
			iterable->registerClasses();
			toLoop->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			iterable->registerFunctionPrototype(r);
			toLoop->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			iterable->buildFunction(r);
			toLoop->buildFunction(r);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			id.error("For-each cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override{
			return &voidClass;
		}
		const E_GEN* setUp(RData& ra) const{
			/*if(iterable->getToken()==T_FUNC_CALL){
				E_FUNC_CALL* func = (E_FUNC_CALL*)iterable;
				std::pair<std::vector<Value*>,const Data* > tempVal = func->getArgs(ra);
				std::vector<Value*> &Args = tempVal.first;
				const Data* d_callee = tempVal.second;
				if(d_callee->type==R_GEN){
					E_GEN* myGen = d_callee.getMyGenerator();
					for(unsigned int i=0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->pointer.setObject(
														new ConstantData(Args[i], myGen->prototype->declarations[i].declarationType));

					}
					return myGen;
				}
			}
			//*/

			///*
			const Data* toEv = iterable->evaluate(ra);
			const AbstractClass* iterC = toEv->getReturnType();
			if(iterC->classType!=CLASS_GEN){
				toEv = getLocalFunction(ra, filePos,"iterator",toEv, NO_TEMPLATE, {});
				iterC = toEv->getReturnType();
				assert(iterC->classType==CLASS_GEN);
			}
			const GeneratorClass* gen = (const GeneratorClass*)iterC;


			//todo create
			const E_GEN* myGen = gen->myGen;

			if(myGen->surroundingClass && !myGen->staticF){
				myGen->module.setVariable(filePos, "this",gen->getLocalData(ra, filePos,"this",toEv));
			}

			for (unsigned Idx = 0; Idx < myGen->declaration.size(); Idx++) {
				const Data* dat = gen->getLocalData(ra, filePos, gen->innerTypes[Idx].second,toEv);
				assert(dat->getReturnType()==myGen->myFunction->getSingleProto()->declarations[Idx].declarationType);
				myGen->declaration[Idx]->variable.getMetadata().setObject(dat);
			}
			return myGen;
			//*/
		}
		const Data* evaluate(RData& ra) const override final{
			//TODO instantly learn if calling "for i in range(3)", no need to create range-object
			auto myGen = setUp(ra);
			myGen->buildFunction(ra);
			auto theClass = myGen->myFunction->getSingleProto()->returnType;
			Jumpable j(name, GENERATOR, nullptr,NULL, NULL, theClass);
			ra.addJump(&j);
			myGen->methodBody->evaluate(ra);
			if(ra.popJump()!= &j) error("Did not receive same func jumpable created (j foreach)");
			//Function* TheFunction;
			llvm::BasicBlock *END = ra.CreateBlock("endLoop");
			if(!ra.hadBreak()) ra.builder.CreateBr(END);
			if(j.endings.size()==1){
				std::pair<llvm::BasicBlock*,llvm::BasicBlock*> NEXT = j.resumes[0];
				ra.builder.SetInsertPoint(NEXT.first);
				const Data* v = j.endings[0].second;
				localVariable->pointer.setObject(v->toValue(ra, filePos));
				assert(NEXT.second);
				assert(END);
				Jumpable k(name, LOOP, /*NO SCOPE -- force iterable to deconstruct*/
						nullptr, NEXT.second, END, NULL);
				ra.addJump(&k);
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k foreach)");
				if(!ra.hadBreak()) ra.builder.CreateBr(NEXT.second);
			}
			else{
				llvm::Type* functionReturnType = theClass->type;
				llvm::BasicBlock* INLOOP = ra.CreateBlock("toLoop");
				llvm::BasicBlock* TODECIDE = ra.CreateBlock("decide");
				ra.builder.SetInsertPoint(INLOOP);
				llvm::PHINode* val = ra.CreatePHI(functionReturnType, (unsigned)(j.endings.size()),"val");
				llvm::PHINode* ind = ra.CreatePHI(llvm::IntegerType::get(llvm::getGlobalContext(),32), (unsigned)(j.endings.size()),"ind");
				localVariable->pointer.setObject(new ConstantData(val, theClass));
				assert(TODECIDE);
				assert(END);
				Jumpable k(name, LOOP, /*NO SCOPE -- force iterable to deconstruct*/
						nullptr, TODECIDE, END, NULL);
				ra.addJump(&k);
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k2 foreach)");
				if(!ra.hadBreak()) ra.builder.CreateBr(TODECIDE);
				ra.builder.SetInsertPoint(TODECIDE);
				llvm::SwitchInst* swit = ra.builder.CreateSwitch(ind, END, (unsigned)(j.endings.size()));
				for(unsigned int i = 0; i<j.endings.size(); i++){
					std::pair<llvm::BasicBlock*,llvm::BasicBlock*> NEXT = j.resumes[i];
					ra.builder.SetInsertPoint(NEXT.first);
					llvm::Value* v = (j.endings[i].second)->getValue(ra,filePos);
					val->addIncoming(v, NEXT.first);
					ind->addIncoming(getInt32(i), NEXT.first);
					ra.builder.CreateBr(INLOOP);
					swit->addCase(getInt32(i), NEXT.second);
				}
			}
			ra.builder.SetInsertPoint(END);

			return &VOID_DATA;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
			toLoop->collectReturns(vals, toBe);
		}
		const Token getToken() const override {
			return T_FOREACH;
		}
};


#endif /* FOREACHLOOP_HPP_ */
