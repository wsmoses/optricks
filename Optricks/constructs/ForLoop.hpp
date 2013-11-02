/*
 * ForLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FORLOOP_HPPO_
#define FORLOOP_HPPO_

#include "./Statement.hpp"
#include "./ForEachLoop.hpp"
class ForLoop : public Construct{
	public:
		Statement* initialize;
		Statement* condition;
		Statement* increment;
		Statement* toLoop;
		String name;
		virtual ~ForLoop(){};
		ForLoop(PositionID a, Statement* init, Statement* cond, Statement* inc,Statement* tL, String n="") :
			Construct(a, voidClass), initialize(init),condition(cond),increment(inc),toLoop(tL){
			name = n;
		}
		ClassProto* checkTypes(RData& r){
			initialize->checkTypes(r);
			condition->checkTypes(r);
			if(condition->returnType != boolClass) error("Cannot have non-bool condition of for-loop "+condition->returnType->name);
			increment->checkTypes(r);
			toLoop->checkTypes(r);
			return returnType;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{
			toLoop->collectReturns(r, vals,toBe);
		}
		const Token getToken() const override {
			return T_FOR;
		}
		DATA evaluate(RData& r) override{
			if(initialize!=NULL && initialize->getToken()!= T_VOID) initialize->evaluate(r);
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();

			BasicBlock *loopBlock = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);
			BasicBlock *incBlock = BasicBlock::Create(getGlobalContext(), "inc", TheFunction);
			BasicBlock *afterBlock = BasicBlock::Create(getGlobalContext(), "endLoop", TheFunction);

			Value *Cond1 = condition->evaluate(r).getValue(r);
			r.builder.CreateCondBr(Cond1, loopBlock, afterBlock);

			r.builder.SetInsertPoint(loopBlock);
			Jumpable* j = new Jumpable(name, LOOP, incBlock, afterBlock, NULL);
			r.addJump(j);
			r.guarenteedReturn = false;
			toLoop->evaluate(r);
			if(r.popJump()!=j) error("Did not receive same func jumpable created");
			r.builder.CreateBr(incBlock);

			r.builder.SetInsertPoint(incBlock);
			if(increment!=NULL && increment->getToken()!= T_VOID) increment->evaluate(r);
			Value *EndCond = condition->evaluate(r).getValue(r);
			if(!r.guarenteedReturn) r.builder.CreateCondBr(EndCond, loopBlock, afterBlock);
			r.guarenteedReturn = false;

			r.builder.SetInsertPoint(afterBlock);
			return DATA::getNull();
		}
		void write(ostream& a, String b="") const override{
			a << "for(" << initialize << "; "<< condition << "; " << increment << ")";
			toLoop->write(a,b);
		}

		void registerClasses(RData& r) override final{
			condition->registerClasses(r);
			initialize->registerClasses(r);
			increment->registerClasses(r);
			toLoop->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			condition->registerFunctionPrototype(r);
			initialize->registerFunctionPrototype(r);
			increment->registerFunctionPrototype(r);
			toLoop->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			condition->buildFunction(r);
			initialize->buildFunction(r);
			increment->buildFunction(r);
			toLoop->buildFunction(r);
		}
		ForLoop* simplify() override{
			return new ForLoop(filePos, initialize->simplify(), condition->simplify(),increment->simplify(),toLoop->simplify(),name);
		}
};


#endif /* FORLOOP_HPP_ */
