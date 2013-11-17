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
			E_GEN* myGen = (E_GEN*)tmp.getPointer();
			if(myGen==NULL) error("Generator is null...");
			myGen->registerFunctionPrototype(r);
			myGen->buildFunction(r);
			theClass = myGen->prototype->returnType;
			localVariable->getMetadata(r)->setObject(DATA::getConstant(NULL,theClass));
			return voidClass;
		}
		DATA evaluate(RData& ra) override final{
			//TODO instantly learn if calling "for i in range(3)", no need to create range-object
			checkTypes(ra);

			E_GEN* myGen = NULL;
			DATA toEv = iterable->evaluate(ra);
			iterC = toEv.getReturnType(ra);
			//TODO do not create struct if calling generator function
			myGen = (E_GEN*)(iterC->getFunction("iterator", filePos)->funcs.get(new FunctionProto("iterator"), ra, filePos).getPointer());
			/*if(E_FUNC_CALL* func = d ynamic_cast<E_FUNC_CALL*>(iterable)){
				auto tmpVal = func->getArgs(ra);
				toEv = tmpVal.second;
				for(const Value*& a: tmpVal.first){

				}
			}*/

			if(iterC->isGen){
				Value* tv = toEv.getValue(ra,filePos);
				if(myGen->thisPointer!=NULL){
					ClassProto* genClass = myGen->self->getSelfClass(ra);
					assert(genClass!=NULL);
					DATA self = DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(0)), genClass);
					if(iterC->layoutType==PRIMITIVEPOINTER_LAYOUT || iterC->layoutType==POINTER_LAYOUT) myGen->thisPointer->setObject(self);
					else myGen->thisPointer->setObject(self.toLocation(ra));

					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->getMetadata(ra)->setObject(
								DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(i+1)), iterC->getDataClass(i+1, filePos)));
					}
				}
				else{
					for(unsigned int i = 0; i<myGen->prototype->declarations.size(); ++i){
						myGen->prototype->declarations[i]->variable->getMetadata(ra)->setObject(
								DATA::getConstant(ra.builder.CreateExtractValue(tv, ArrayRef<unsigned>(i)),iterC->getDataClass(i, filePos)));
					}
				}
			} else{
				if(myGen->thisPointer!=NULL){
					if(iterC->layoutType==PRIMITIVEPOINTER_LAYOUT || iterC->layoutType==POINTER_LAYOUT) myGen->thisPointer->setObject(toEv.toValue(ra,filePos));
					else myGen->thisPointer->setObject(toEv.toLocation(ra));
				}
				for(auto& a: myGen->prototype->declarations){
					if(a->value==NULL || a->value->getToken()==T_VOID) error("iterator generator has non-optional arguments");
					a->variable->getMetadata(ra)->setObject(a->value->evaluate(ra));
				}
			}
			Jumpable j("", GENERATOR, NULL, NULL, theClass);
			ra.addJump(&j);
			myGen->ret->evaluate(ra);
			if(ra.popJump()!= &j) error("Did not receive same func jumpable created (j foreach)");
			//Function* TheFunction;
			BasicBlock *END = ra.CreateBlock("endLoop");
			ra.builder.CreateBr(END);

			toLoop->checkTypes(ra);
			if(j.endings.size()==1){
				std::pair<BasicBlock*,BasicBlock*> NEXT = j.resumes[0];
				ra.builder.SetInsertPoint(NEXT.first);
				DATA v = j.endings[0].second;
				if(!(v.getType()==R_LOC || v.getType()==R_CONST)) filePos.error("Cannot use object designated as "+str<DataType>(v.getType())+" for iterable");
				localVariable->getMetadata(ra)->setObject(v.toLocation(ra));
				assert(NEXT.second);
				assert(END);
				Jumpable k(name, LOOP, NEXT.second, END, NULL);
				ra.addJump(&k);
				ra.guarenteedReturn = false;
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k foreach)");
				if(!ra.guarenteedReturn) ra.builder.CreateBr(NEXT.second);
			}
			else{
				Type* functionReturnType = theClass->getType(ra);
				BasicBlock* INLOOP = ra.CreateBlock("toLoop");
				BasicBlock* TODECIDE = ra.CreateBlock("decide");
				ra.builder.SetInsertPoint(INLOOP);
				PHINode* val = ra.CreatePHI(functionReturnType, (unsigned)(j.endings.size()),"val");
				PHINode* ind = ra.CreatePHI(INT32TYPE, (unsigned)(j.endings.size()),"ind");
				auto met = localVariable->getMetadata(ra);
				met->setObject(DATA::getConstant(val, theClass));
				localVariable->returnType = theClass;
				assert(TODECIDE);
				assert(END);
				Jumpable k(name, LOOP, TODECIDE, END, NULL);
				ra.addJump(&k);
				ra.guarenteedReturn = false;
				toLoop->evaluate(ra);
				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k2 foreach)");
				if(!ra.guarenteedReturn) ra.builder.CreateBr(TODECIDE);
				ra.builder.SetInsertPoint(TODECIDE);
				SwitchInst* swit = ra.builder.CreateSwitch(ind, END, (unsigned)(j.endings.size()));
				for(unsigned int i = 0; i<j.endings.size(); i++){
					std::pair<BasicBlock*,BasicBlock*> NEXT = j.resumes[i];
					ra.builder.SetInsertPoint(NEXT.first);
					Value* v = (j.endings[i].second).getValue(ra,filePos);
					val->addIncoming(v, NEXT.first);
					ind->addIncoming(getInt32(i), NEXT.first);
					ra.builder.CreateBr(INLOOP);
					swit->addCase(getInt32(i), NEXT.second);
				}
			}
			ra.builder.SetInsertPoint(END);
			return DATA::getNull();
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{
			toLoop->collectReturns(r, vals, toBe);
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
