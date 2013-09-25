/*
 * WhileLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef WHILELOOP_HPPO_
#define WHILELOOP_HPPO_

#include "./Statement.hpp"

class DoWhileLoop : public Construct{
	public:
		Statement* const condition;
		Statement* const statement;
		String name;
		virtual ~DoWhileLoop(){};
		DoWhileLoop(PositionID a, Statement * cond, Statement* stat,String n="") :
			Construct(a, voidClass),
			condition(cond), statement(stat){
			name = n;
		}
		ClassProto* checkTypes(RData& r) override final{
			condition->checkTypes(r);
			if(condition->returnType!=boolClass) error("Cannot make non-bool type condition for while loop");
			statement->checkTypes(r);
			return returnType;
		}
		const Token getToken() const override {
			return T_WHILE;
		}
		DATA evaluate(RData& r) override{
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();

			BasicBlock *loopBlock = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);
			BasicBlock *incBlock = BasicBlock::Create(getGlobalContext(), "inc", TheFunction);
			BasicBlock *afterBlock = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);

			r.builder.CreateBr(loopBlock);

			// Start insertion in LoopBB.
			r.builder.SetInsertPoint(loopBlock);
			Jumpable* j = new Jumpable(name, LOOP, incBlock, afterBlock, NULL);
			r.addJump(j);
			r.guarenteedReturn = false;
			statement->evaluate(r);
			if(r.popJump()!=j) error("Did not receive same while-loop jumpable created");
			r.builder.CreateBr(incBlock);


			r.builder.SetInsertPoint(incBlock);
			Value *EndCond = condition->evaluate(r);
			if(!r.guarenteedReturn) r.builder.CreateCondBr(EndCond, loopBlock, afterBlock);
			r.guarenteedReturn = false;

			r.builder.SetInsertPoint(afterBlock);
			return NULL;
		}

		void registerClasses(RData& r) override final{
			condition->registerClasses(r);
			statement->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			condition->registerFunctionArgs(r);
			statement->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			condition->registerFunctionDefaultArgs();
			statement->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			condition->resolvePointers();
			statement->resolvePointers();
		}
		void write(ostream& a, String b) const override{
			a << "while(" << condition << ")";
			statement->write(a,b);
		}
		DoWhileLoop* simplify() override final{
			return new DoWhileLoop(filePos, condition->simplify(), statement->simplify(), name);
		}
};


#endif /* WHILELOOP_HPP_ */
