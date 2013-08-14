/*
 * ForLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FORLOOP_HPPO_
#define FORLOOP_HPPO_

#include "./Expression.hpp"
class ForLoop : public Statement{
	public:
		Statement* initialize;
		Expression* condition;
		Statement* increment;
		Statement* toLoop;
		String name;
		ForLoop(Statement* init, Expression* cond, Statement* inc,Statement* tL, String n="") :
			initialize(init),condition(cond),increment(inc),toLoop(tL){
			/*if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			name = n;
		}
		void checkTypes(){
			initialize->checkTypes();
			condition->checkTypes();
			if(condition->returnType != boolClass) todo("Cannot have non-bool condition of for-loop ",condition->returnType->name);
			increment->checkTypes();
			toLoop->checkTypes();
		}
		const Token getToken() const override {
			return T_FOR;
		}
		Value* evaluate(RData& r, LLVMContext& context) override{
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
		Statement* simplify(Jump& jump) override{
			Statement* in = toLoop->simplify(jump);
			jump = NJUMP;
			Statement* init = initialize->simplify(jump);
			jump = NJUMP;
			Statement* inc = increment->simplify(jump);
			//if(jump.type==BREAK && (jump.label=="" || jump.label==name))
			jump = NJUMP;
			return new ForLoop(init, condition->simplify(),inc,in,name);
			//TODO [loop unrolloing]
		}
};


#endif /* FORLOOP_HPP_ */
