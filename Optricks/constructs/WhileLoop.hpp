/*
 * WhileLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef WHILELOOP_HPPO_
#define WHILELOOP_HPPO_

#include "./Statement.hpp"

class WhileLoop : public Statement{
	public:
		Statement* const condition;
		Statement* const statement;
		String name;
		WhileLoop(PositionID a, Statement * cond, Statement* stat,String n="") :
			Statement(a, voidClass),
			condition(cond), statement(stat){
			name = n;
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		ClassProto* checkTypes() override final{
			condition->checkTypes();
			if(condition->returnType!=boolClass) todo("Cannot make non-bool type condition for while loop");
			statement->checkTypes();
			return returnType;
		}
		const Token getToken() const override {
			return T_WHILE;
		}
		Value* evaluate(RData& r) override{
			todo("While loop eval not implemented");
			/*
			while((bool)condition->evaluate()){
				statement->evaluate(jump);
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
			}
			return VOID;
			*/
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
		WhileLoop* simplify() override final{
			return new WhileLoop(filePos, condition->simplify(), statement->simplify(), name);
		}
};


#endif /* WHILELOOP_HPP_ */
