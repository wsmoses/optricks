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

class ForEachLoop : public ErrorStatement{
	public:
		AbstractClass* theClass;
		E_VAR* localVariable;
		Statement* iterable;
		Statement* toLoop;
		String name;
		ForEachLoop(PositionID id, E_VAR* var, Statement* it,Statement* tL, String n="") :
			ErrorStatement(id), theClass(NULL),localVariable(var), iterable(it),toLoop(tL){
			name = n;
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

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
			id.error("For-each cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override{
			error("Cannot getReturnType of FOR-EACH");
			exit(1);
		}
		E_GEN* setUp(RData& ra) const{
			filePos.compilerError("For-each loops not implemented");
			exit(1);
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
			}*/

			/*
			const Data* toEv = iterable->evaluate(ra);
			const AbstractClass* iterC = toEv->getReturnType();
			E_GEN* myGen = (E_GEN*)(iterC->getLocalFunction(filePos,"iterator",std::vector<const AbstractClass*>()).getPointer());

			if(iterC->isGen){
				Value* tv = toEv->getValue(ra,filePos);
				if(myGen->thisPointer.module!=NULL){
					const AbstractClass* genClass = myGen->self->getSelfClass(filePos);
					assert(genClass!=NULL);
					Data* self = new ConstantData(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(0)), genClass);
					if(iterC->layout==PRIMITIVEPOINTER_LAYOUT || iterC->layout==POINTER_LAYOUT) myGen->thisPointer.setObject(self);
					else myGen->thisPointer.setObject(self->toLocation(ra));

					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->getMetadata().setObject(
								new ConstantData(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(i+1)), iterC->getDataClass(i+1, filePos)));
					}
				}
				else{
					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->pointer.setObject(
								new ConstantData(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(i)),iterC->getDataClass(i, filePos)));
					}
				}
			} else{
				if(myGen->thisPointer.module!=NULL){
					if(iterC->layout==PRIMITIVEPOINTER_LAYOUT || iterC->layout==POINTER_LAYOUT)
						myGen->thisPointer.setObject(toEv->toValue(ra,filePos));
					else myGen->thisPointer.setObject(toEv->toLocation(ra));
				}
				for(auto& a: myGen->prototype->declarations){
					if(a->value==NULL || a->value->getToken()==T_VOID) error("iterator generator has non-optional arguments");
					a->variable->pointer.setObject(a->value->evaluate(ra));
				}
			}
			return myGen;
			*/
		}
		const Data* evaluate(RData& ra) const override final{
			//TODO instantly learn if calling "for i in range(3)", no need to create range-object

			E_GEN* myGen = setUp(ra);
			myGen->buildFunction(ra);
			Jumpable j("", GENERATOR, NULL, NULL, theClass);
			ra.addJump(&j);
			myGen->body->evaluate(ra);
			if(ra.popJump()!= &j) error("Did not receive same func jumpable created (j foreach)");
			//Function* TheFunction;
			BasicBlock *END = ra.CreateBlock("endLoop");
			ra.builder.CreateBr(END);
			if(j.endings.size()==1){
				std::pair<BasicBlock*,BasicBlock*> NEXT = j.resumes[0];
				ra.builder.SetInsertPoint(NEXT.first);
				const Data* v = j.endings[0].second;
				//todo -- remove this
				if(!(v->type==R_LOC || v->type==R_CONST))
					filePos.error("Cannot use object designated as "+str(v->type)+" for iterable");
				localVariable->pointer.setObject(v);//todo removed toLocation -- check if needed
				assert(NEXT.second);
				assert(END);
				Jumpable k(name, LOOP, NEXT.second, END, NULL);
				ra.addJump(&k);
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k foreach)");
				if(!ra.hadBreak()) ra.builder.CreateBr(NEXT.second);
			}
			else{
				Type* functionReturnType = theClass->type;
				BasicBlock* INLOOP = ra.CreateBlock("toLoop");
				BasicBlock* TODECIDE = ra.CreateBlock("decide");
				ra.builder.SetInsertPoint(INLOOP);
				PHINode* val = ra.CreatePHI(functionReturnType, (unsigned)(j.endings.size()),"val");
				PHINode* ind = ra.CreatePHI(IntegerType::get(getGlobalContext(),32), (unsigned)(j.endings.size()),"ind");
				localVariable->pointer.setObject(new ConstantData(val, theClass));
				assert(TODECIDE);
				assert(END);
				Jumpable k(name, LOOP, TODECIDE, END, NULL);
				ra.addJump(&k);
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k2 foreach)");
				if(!ra.hadBreak()) ra.builder.CreateBr(TODECIDE);
				ra.builder.SetInsertPoint(TODECIDE);
				SwitchInst* swit = ra.builder.CreateSwitch(ind, END, (unsigned)(j.endings.size()));
				for(unsigned int i = 0; i<j.endings.size(); i++){
					std::pair<BasicBlock*,BasicBlock*> NEXT = j.resumes[i];
					ra.builder.SetInsertPoint(NEXT.first);
					Value* v = (j.endings[i].second)->getValue(ra,filePos);
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
