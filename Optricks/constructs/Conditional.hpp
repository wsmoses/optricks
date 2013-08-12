/*
 * Conditional.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef CONDITIONAL_HPP_
#define CONDITIONAL_HPP_

#include "./Expression.hpp"

/*
class Conditional : public Statement{
	public:
		Expression* const condition;
		Statement* const statement;
		Conditional(Expression * const cond, Statement* const stat) : condition(cond), statement(stat){
			if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}
		}
		const Token getToken() const override {
			return T_CONDITIONAL;
		}
		void* evaluate(Jump& jump) override{
			auto eval = condition->evaluate(jump);
			if(jump!=NOJUMP)
				return eval;
			else if ((bool)eval)
				return statement->evaluate(jump);
			else return VOID;
		}
		Statement* simplify(Jump& jump) override{
			if(condition->getToken()==T_OOBJECT){
				obool* c;
				c = (obool*)condition;
				if( (bool)c ){
					return statement->simplify();
				}
				else return VOID;
			}
			else{
				Statement* in = statement->simplify();
				if(in->getToken()==T_VOID){
					return condition->simplify();
				} else return new Conditional(condition->simplify(), in);
			}
		}
};
*/

#endif /* CONDITIONAL_HPP_ */
