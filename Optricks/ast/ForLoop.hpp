/*
 * ForLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */


#ifndef FORLOOP_HPPO_
#define FORLOOP_HPPO_

#include "../language/statement/Statement.hpp"
#include "./ForEachLoop.hpp"
class ForLoop : public ErrorStatement{
	public:
		Statement* initialize;
		Statement* condition;
		Statement* increment;
		Statement* toLoop;
		String name;
		virtual ~ForLoop(){};
		ForLoop(PositionID a, Statement* init, Statement* cond, Statement* inc,Statement* tL, String n="") :
			ErrorStatement(a), initialize(init),condition(cond),increment(inc),toLoop(tL){
			name = n;
		}
		const AbstractClass* getReturnType() const override final{
			error("Cannot getReturnType of FOR");
			exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
			toLoop->collectReturns(vals,toBe);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
			id.error("for-loop cannot act as function");
			exit(1);
		}
		const Token getToken() const override {
			return T_FOR;
		}
		const Data* evaluate(RData& r) const override{
			if(initialize!=NULL && initialize->getToken()!= T_VOID) initialize->evaluate(r);
			BasicBlock *loopBlock;
			BasicBlock *incBlock;
			BasicBlock *afterBlock;

			Value *Cond1 = condition->evaluate(r)->castToV(r,boolClass,filePos);
			//BasicBlock* StartBB = r.builder.GetInsertBlock();
			if(ConstantInt* c = dyn_cast<ConstantInt>(Cond1)){
				if(c->isOne()){
					loopBlock = r.CreateBlock("loop");
					afterBlock = r.CreateBlock("endLoop");
					r.builder.CreateBr(loopBlock);
				}
				else{
					return VOID_DATA;
				}
			} else{
				loopBlock = r.CreateBlock("loop");
				afterBlock = r.CreateBlock("endLoop");
				r.builder.CreateCondBr(Cond1, loopBlock, afterBlock);
			}
			incBlock = r.CreateBlock("inc");
			r.builder.SetInsertPoint(loopBlock);
			assert(incBlock); assert(afterBlock);
			Jumpable j(name, LOOP, incBlock, afterBlock, NULL);
			r.addJump(&j);
			toLoop->evaluate(r);
#ifndef NDEBUG
			auto tmp = r.popJump();
			assert(tmp== &j);
#else
			r.popJump();
#endif
			r.builder.CreateBr(incBlock);

			r.builder.SetInsertPoint(incBlock);
			if(increment!=NULL && increment->getToken()!= T_VOID) increment->evaluate(r);
			Value *EndCond = condition->evaluate(r)->castToV(r,boolClass,filePos);
			if(!r.hadBreak()){
				if(ConstantInt* c = dyn_cast<ConstantInt>(EndCond)){
					if(c->isOne()) r.builder.CreateBr(loopBlock);
					else r.builder.CreateBr(afterBlock);
				} else {
					r.builder.CreateCondBr(EndCond, loopBlock, afterBlock);
				}
			}

			r.builder.SetInsertPoint(afterBlock);
			return VOID_DATA;
		}
		void registerClasses() const override final{
			condition->registerClasses();
			initialize->registerClasses();
			increment->registerClasses();
			toLoop->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			condition->registerFunctionPrototype(r);
			initialize->registerFunctionPrototype(r);
			increment->registerFunctionPrototype(r);
			toLoop->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			condition->buildFunction(r);
			initialize->buildFunction(r);
			increment->buildFunction(r);
			toLoop->buildFunction(r);
		}
};
#endif /* FORLOOP_HPP_ */
