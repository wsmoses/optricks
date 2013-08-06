/*
 * WhileLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef WHILELOOP_HPPO_
#define WHILELOOP_HPPO_

#include "./Expression.hpp"

class WhileLoop : public Statement{
	public:
		Expression* const condition;
		Statement* const statement;
		String name;
		WhileLoop(Expression * cond, Statement* stat,String n="") : condition(cond), statement(stat){
			if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}
			name = n;
		}
		const Token getToken() const override {
			return T_WHILE;
		}
		void* evaluate(Jump& jump) override{
			while((bool)condition->evaluate()){
				statement->evaluate(jump);
				switch(jump.type){
					case NJUMP:
						break;
					case CONTINUE:
						break;
					case BREAK:
						if(name==jump.label || jump.label=="") jump = NOJUMP;
						return VOID;
					case RETURN:
						return VOID;
				}
			}
			return VOID;
		}
		void write(ostream& a, String b) const override{
			a << "while(" << condition << ")";
			statement->write(a,b);
		}
		Statement* simplify(Jump& jump) override{
			if(condition->getToken()==T_OOBJECT){
				obool* c;
				c = (obool*)condition;
				if( (bool)c ){
					//CHECK for optimizations due to jump
					return new WhileLoop(c,statement->simplify(jump),name);
				}
				else return VOID;
			}
			else{
				Statement* in = statement->simplify(jump);
				//if(in->getToken()==T_VOID){
				//	return condition->simplify();
				//}
				//else
					return new WhileLoop(condition->simplify(), in,name);
			}
		}
};


#endif /* WHILELOOP_HPP_ */
