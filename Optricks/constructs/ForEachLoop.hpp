/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HPPO_
#define FOREACHLOOP_HPPO_

#include "./Expression.hpp"
//TODO implement iterator
class ForEachLoop : public Statement{
	public:
		String localVariableName;
		Expression* iterable;
		Statement* toLoop;
		String name;
		ForEachLoop(String& var, Expression* it,Statement* tL, String n="") :
			localVariableName(var), iterable(it),toLoop(tL){
			/*if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			name = n;
		}
		const Token getToken() const override {
			return T_FOREACH;
		}
		Value* evaluate(RData& a, LLVMContext& context) override{
			//TODO iterators not implemented yet
			todo("For-each loop eval not implented");
/*			while((bool)condition->evaluate()){
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
			}*/
		}
		void write(ostream& a, String b="") const override{
			a << "for " << localVariableName << " in "<< iterable << ":";
			toLoop->write(a,b+"  ");
		}
		Statement* simplify(Jump& jump) override{
			Statement* in = toLoop->simplify(jump);
			//if(jump.type==BREAK && (jump.label=="" || jump.label==name))
			jump = NJUMP;
			return new ForEachLoop(localVariableName, iterable->simplify(),in,name);
			//TODO [loop unrolloing]
		}
};


#endif /* FOREACHLOOP_HPP_ */
