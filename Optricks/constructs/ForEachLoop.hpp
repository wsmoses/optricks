/*
 * ForEachLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef FOREACHLOOP_HPPO_
#define FOREACHLOOP_HPPO_

#include "./Statement.hpp"
//TODO implement iterator
class ForEachLoop : public Construct{
	public:
		E_VAR* localVariable;
		Statement* iterable;
		Statement* toLoop;
		String name;
		virtual ~ForEachLoop();
		ForEachLoop(PositionID id, E_VAR* var, Statement* it,Statement* tL, String n="") :
			Construct(id, voidClass), localVariable(var), iterable(it),toLoop(tL){
			/*if(condition->returnType!=boolClass){
				cerr << "Cannot make non-bool type argument of conditional" << endl << flush;
				exit(0);
			}*/
			name = n;
		}

		ClassProto* checkTypes(RData& r){
			iterable->checkTypes(r);
			toLoop->checkTypes(r);
			error("Type checking for foreach incomplete due to lack of iterator");
			return returnType;
		}
		const Token getToken() const override {
			return T_FOREACH;
		}
		DATA evaluate(RData& a) override{
			//TODO iterators not implemented yet
			error("For-each loop eval not implented");
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
			return NULL;
		}
		void write(ostream& a, String b="") const override{
			a << "for " << localVariable << " in "<< iterable << ":";
			toLoop->write(a,b+"  ");
		}
		Statement* simplify() override{
			Statement* in = toLoop->simplify();
			//if(jump.type==BREAK && (jump.label=="" || jump.label==name))
			error("Cannot simplify foreach loop");
			//return new ForEachLoop(filePos, localVariable, iterable->simplify(),in,name);
			//TODO [loop unrolloing]
			return NULL;
		}
};


#endif /* FOREACHLOOP_HPP_ */
