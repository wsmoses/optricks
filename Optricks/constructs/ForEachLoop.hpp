/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HO_
#define FOREACHLOOP_HO_

#include "./Statement.hpp"

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
		void registerFunctionDefaultArgs() override final{
			iterable->registerFunctionDefaultArgs();
			toLoop->registerFunctionDefaultArgs();
		}
		DATA evaluate(RData& ra) override final{
			checkTypes(ra);
			Function *TheFunction = ra.builder.GetInsertBlock()->getParent();
			//BasicBlock *Parent = ra.builder.GetInsertBlock();
			//BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "eachLoop", TheFunction);
			BasicBlock *END = BasicBlock::Create(getGlobalContext(), "endLoop", TheFunction);

			if(iterC->isPointer) iterC->iterator->thisPointer->setValue(iterable->evaluate(ra),ra);
			else{
				iterC->iterator->thisPointer->llvmLocation = iterable->getLocation(ra);
			if(iterC->iterator->thisPointer->llvmLocation==NULL){
				iterC->iterator->thisPointer->llvmLocation = ra.builder.CreateAlloca(iterable->checkTypes(ra)->getType(ra), 0);
				ra.builder.CreateStore(
						iterable->evaluate(ra)
						,iterC->iterator->thisPointer->llvmLocation);
			}
			}

			Jumpable* j = new Jumpable("", GENERATOR, NULL, NULL, theClass);
			ra.addJump(j);
			iterC->iterator->ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			ra.builder.CreateBr(END);

			//TheFunction->getBasicBlockList().remove(MERGE);
			//TheFunction->getBasicBlockList().push_back(MERGE);
			//ra.builder.SetInsertPoint(MERGE);

			Type* functionReturnType = theClass->getType(ra);
			for(unsigned int i = 0; i<j->endings.size(); i++){
				std::pair<BasicBlock*,BasicBlock*> NEXT = j->resumes[i];
				ra.builder.SetInsertPoint(NEXT.first);
				DATA v = j->endings[i].second;
					DATA loc = ra.builder.CreateAlloca(functionReturnType,0);
					ra.builder.CreateStore(v, loc);
					localVariable->getMetadata(ra)->llvmLocation = loc;
				//TODO end, continue

				Jumpable* k = new Jumpable(name, LOOP, NEXT.second, END, NULL);
				ra.addJump(k);
				ra.guarenteedReturn = false;
				toLoop->evaluate(ra);
				if(ra.popJump()!=k) error("Did not receive same func jumpable created");

				ra.builder.CreateBr(NEXT.second);
			}
			TheFunction->getBasicBlockList().remove(END);
			TheFunction->getBasicBlockList().push_back(END);
			ra.builder.SetInsertPoint(END);
			return NULL;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
			toLoop->collectReturns(r, vals);
		}
		ClassProto* checkTypes(RData& r) override final{
			iterC = iterable->checkTypes(r);
			if(iterC->iterator==NULL) error("Could not find iterator for class: "+iterC->name);
			std::vector<ClassProto*> yields;
			iterC->iterator->ret->collectReturns(r, yields);
			theClass = getMin(yields,filePos);
			localVariable->getMetadata(r)->returnClass = theClass;
			toLoop->checkTypes(r);
			return voidClass;
		}
		void resolvePointers() override final{
			iterable->resolvePointers();
			toLoop->resolvePointers();
		}
		void registerFunctionArgs(RData& r) override final{
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
