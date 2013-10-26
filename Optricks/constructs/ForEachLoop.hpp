/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HO_
#define FOREACHLOOP_HO_

#include "./Statement.hpp"
#include "./Declaration.hpp"
#include "../expressions/E_GEN.hpp"

class ForEachLoop : public Construct{
	public:
		ClassProto* theClass;
		ClassProto* iterC;
		E_VAR* localVariable;
		Statement* iterable;
		Statement* toLoop;
		String name;
		ForEachLoop(PositionID id, E_VAR* var, Statement* it,Statement* tL, String n="") :
			Construct(id, voidClass), theClass(NULL),iterC(NULL),localVariable(var), iterable(it),toLoop(tL){
			name = n;
		}

		void registerClasses(RData& r) override final{
			iterable->registerClasses(r);
			toLoop->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			iterable->registerFunctionPrototype(r);
			toLoop->registerFunctionPrototype(r);
		}
		ClassProto* checkTypes(RData& r) override final{
			iterC = iterable->checkTypes(r);
			ReferenceElement* ref = iterC->getFunction("iterator", filePos);
			FunctionProto* f = new FunctionProto("for-each-temp");
			DATA tmp = ref->funcs.get(f, r, filePos);
			if(tmp.getType()!=R_GEN) error("Cannot for-each on non-generator");
			std::vector<ClassProto*> yields;
			E_GEN* myGen = tmp.getMyGenerator();
			myGen->registerFunctionPrototype(r);
			myGen->buildFunction(r);
			myGen->ret->collectReturns(r, yields);
			theClass = getMin(yields,filePos);
			localVariable->getMetadata(r)->llvmObject = DATA::getConstant(NULL,theClass);
			return voidClass;
		}
		DATA evaluate(RData& ra) override final{
			//TODO instantly learn if calling "for i in range(3)", no need to create range-object
			checkTypes(ra);
			Function *TheFunction = ra.builder.GetInsertBlock()->getParent();
			//BasicBlock *Parent = ra.builder.GetInsertBlock();
			//BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "eachLoop", TheFunction);
			BasicBlock *END = BasicBlock::Create(getGlobalContext(), "endLoop", TheFunction);
			//DATA toEv = DATA::getNull();
			E_GEN* myGen = NULL;
			DATA toEv = iterable->evaluate(ra);
			//TODO do not create struct if calling generator function
			//iterC = toEv.getReturnType();
			myGen = iterC->getFunction("iterator", filePos)->funcs.get(new FunctionProto("iterator"), ra, filePos).getMyGenerator();
			/*if(E_FUNC_CALL* func = d ynamic_cast<E_FUNC_CALL*>(iterable)){
				auto tmpVal = func->getArgs(ra);
				toEv = tmpVal.second;
				for(const Value*& a: tmpVal.first){

				}
			}*/

			if(iterC->isGen){
				Value* tv = toEv.getValue(ra);
				if(myGen->thisPointer!=NULL){
					ClassProto* genClass = myGen->self->getSelfClass(ra);
					assert(genClass!=NULL);
					DATA self = DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(std::vector<unsigned>({0}))), genClass);
					if(iterC->isPointer) myGen->thisPointer->llvmObject = self;
					else myGen->thisPointer->llvmObject = self.toLocation(ra);

					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->getMetadata(ra)->llvmObject =
								DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(std::vector<unsigned>({i+1}))), iterC->getDataClass(i+1, filePos));
					}
				}
				else{
					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->getMetadata(ra)->llvmObject =
								DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(std::vector<unsigned>({i}))),iterC->getDataClass(i, filePos));
					}
				}
			} else{
				if(myGen->thisPointer!=NULL){
					if(iterC->isPointer) myGen->thisPointer->llvmObject = toEv.toValue(ra);
					else myGen->thisPointer->llvmObject = toEv.toLocation(ra);
				}
				for(auto& a: myGen->prototype->declarations){
					if(a->value==NULL || a->value->getToken()==T_VOID) error("iterator generator has non-optional arguments");
					a->variable->getMetadata(ra)->llvmObject = a->value->evaluate(ra);
				}
			}
			Jumpable* j = new Jumpable("", GENERATOR, NULL, NULL, theClass);
			ra.addJump(j);
			myGen->ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			ra.builder.CreateBr(END);

			toLoop->checkTypes(ra);
			if(j->endings.size()==1){
				std::pair<BasicBlock*,BasicBlock*> NEXT = j->resumes[0];
				ra.builder.SetInsertPoint(NEXT.first);
				DATA v = j->endings[0].second;
				if(!(v.getType()==R_LOC || v.getType()==R_CONST)) filePos.error("Cannot use object designated as "+str<DataType>(v.getType())+" for iterable");
				localVariable->getMetadata(ra)->llvmObject = v.toLocation(ra);
				Jumpable* k = new Jumpable(name, LOOP, NEXT.second, END, NULL);
				ra.addJump(k);
				ra.guarenteedReturn = false;
				toLoop->evaluate(ra);
				if(ra.popJump()!=k) error("Did not receive same func jumpable created");
				if(!ra.guarenteedReturn) ra.builder.CreateBr(NEXT.second);
			}
			else{
				Type* functionReturnType = theClass->getType(ra);
				BasicBlock* INLOOP = BasicBlock::Create(getGlobalContext(), "toLoop", TheFunction);
				BasicBlock* TODECIDE = BasicBlock::Create(getGlobalContext(), "decide", TheFunction);
				ra.builder.SetInsertPoint(INLOOP);
				PHINode* val = ra.builder.CreatePHI(functionReturnType, (unsigned)(j->endings.size()),"val");
				PHINode* ind = ra.builder.CreatePHI(INT32TYPE, (unsigned)(j->endings.size()),"ind");
				auto met = localVariable->getMetadata(ra);
				met->llvmObject = DATA::getConstant(val, theClass);
				localVariable->returnType = theClass;
				Jumpable* k = new Jumpable(name, LOOP, TODECIDE, END, NULL);
				ra.addJump(k);
				ra.guarenteedReturn = false;
				toLoop->evaluate(ra);
				if(ra.popJump()!=k) error("Did not receive same func jumpable created");
				if(!ra.guarenteedReturn) ra.builder.CreateBr(TODECIDE);
				ra.builder.SetInsertPoint(TODECIDE);
				SwitchInst* swit = ra.builder.CreateSwitch(ind, END, (unsigned)(j->endings.size()));
				for(unsigned int i = 0; i<j->endings.size(); i++){
					std::pair<BasicBlock*,BasicBlock*> NEXT = j->resumes[i];
					ra.builder.SetInsertPoint(NEXT.first);
					Value* v = (j->endings[i].second).getValue(ra);
					val->addIncoming(v, NEXT.first);
					ind->addIncoming(getInt32(i), NEXT.first);
					ra.builder.CreateBr(INLOOP);
					swit->addCase(getInt32(i), NEXT.second);
				}
			}
			TheFunction->getBasicBlockList().remove(END);
			TheFunction->getBasicBlockList().push_back(END);
			ra.builder.SetInsertPoint(END);
			return DATA::getNull();
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
			toLoop->collectReturns(r, vals);
		}
		void buildFunction(RData& r) override final{
			iterable->buildFunction(r);
			toLoop->buildFunction(r);
		}
		const Token getToken() const override {
			return T_FOREACH;
		}
		void write(ostream& a, String b="") const override{
			a << "for " << localVariable << " in "<< iterable << ":";
			toLoop->write(a,b+"  ");
		}
		Statement* simplify() override{
			return this;
		}
};


#endif /* FOREACHLOOP_HPP_ */
