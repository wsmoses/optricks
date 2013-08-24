/*
 * ForLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FORLOOP_HPPO_
#define FORLOOP_HPPO_

#include "./Statement.hpp"
class ForLoop : public Statement{
	public:
		Statement* initialize;
		Statement* condition;
		Statement* increment;
		Statement* toLoop;
		String name;
		ForLoop(PositionID a, Statement* init, Statement* cond, Statement* inc,Statement* tL, String n="") :
			Statement(a, voidClass), initialize(init),condition(cond),increment(inc),toLoop(tL){
			/*if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			name = n;
		}
		ClassProto* checkTypes(){
			initialize->checkTypes();
			condition->checkTypes();
			if(condition->returnType != boolClass) error("Cannot have non-bool condition of for-loop "+condition->returnType->name);
			increment->checkTypes();
			toLoop->checkTypes();
			return returnType;
		}
		const Token getToken() const override {
			return T_FOR;
		}
		Value* evaluate(RData& r) override{
			if(initialize!=NULL && initialize->getToken()!= T_VOID) initialize->evaluate(r);
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock *PreheaderBB = r.builder.GetInsertBlock();

			BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);

			  // Insert an explicit fall through from the current block to the LoopBB.
			r.builder.CreateBr(LoopBB);

			  // Start insertion in LoopBB.
			r.builder.SetInsertPoint(LoopBB);
			toLoop->evaluate(r);
			if(increment!=NULL && increment->getToken()!= T_VOID) increment->evaluate(r);
			Value *EndCond = condition->evaluate(r);

			BasicBlock *LoopEndBB = r.builder.GetInsertBlock();
			  BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);

			  // Insert the conditional branch into the end of LoopEndBB.
			  r.builder.CreateCondBr(EndCond, LoopBB, AfterBB);

			  // Any new code will be inserted in AfterBB.
			  r.builder.SetInsertPoint(AfterBB);
			  return AfterBB;
		}
		void write(ostream& a, String b="") const override{
			a << "for(" << initialize << "; "<< condition << "; " << increment << ")";
			toLoop->write(a,b+"  ");
		}

		void registerClasses(RData& r) override final{
			condition->registerClasses(r);
			initialize->registerClasses(r);
			increment->registerClasses(r);
			toLoop->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			condition->registerFunctionArgs(r);
			initialize->registerFunctionArgs(r);
			increment->registerFunctionArgs(r);
			toLoop->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			condition->registerFunctionDefaultArgs();
			initialize->registerFunctionDefaultArgs();
			increment->registerFunctionDefaultArgs();
			toLoop->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			condition->resolvePointers();
			initialize->resolvePointers();
			increment->resolvePointers();
			toLoop->resolvePointers();
		}
		ForLoop* simplify() override{
			return new ForLoop(filePos, initialize->simplify(), condition->simplify(),increment->simplify(),toLoop->simplify(),name);
			//TODO [loop unrolloing]
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); }
};


#endif /* FORLOOP_HPP_ */
