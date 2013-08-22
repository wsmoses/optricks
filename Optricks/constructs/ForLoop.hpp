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
		FunctionProto* getFunctionProto() override final{ return NULL; }
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
			if(condition->returnType != boolClass) todo("Cannot have non-bool condition of for-loop ",condition->returnType->name);
			increment->checkTypes();
			toLoop->checkTypes();
			return returnType;
		}
		const Token getToken() const override {
			return T_FOR;
		}
		Value* evaluate(RData& r) override{
			todo("For loop eval not implemented");
			/*
			initialize->evaluate(jump);
			if(jump.type!=NJUMP){
				cerr << "Cannot jump on initialize statement in for loop" << endl;
				exit(0);
			}
			while((bool)condition->evaluate()){
				toLoop->evaluate(jump);
				switch(jump.type){
					case NJUMP:
						break;
					case CONTINUE:
						if(name==jump.label || jump.label=="") jump = NOJUMP;
						break;
					case BREAK:
						if(name==jump.label || jump.label=="") jump = NOJUMP;
						return VOID;
					case RETURN:
						return VOID;
				}
				jump = NJUMP;
				increment->evaluate(jump);
				if(jump.type!=NJUMP){
					cerr << "Cannot jump on increment statement in for loop" << endl;
					exit(0);
				}
			}
			return VOID;*/
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
};


#endif /* FORLOOP_HPP_ */
