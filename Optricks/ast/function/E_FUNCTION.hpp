/*
 * E_FUNCTION.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef E_FUNCTION_HPP_
#define E_FUNCTION_HPP_
#include "../../language/statement/Statement.hpp"
class E_FUNCTION:public ErrorStatement{
	public:
		std::vector<Declaration*> declaration;
		SingleFunction* myFunction;
		E_FUNCTION(PositionID id, std::vector<Declaration*> dec):
			ErrorStatement(id),
				declaration(dec),myFunction(nullptr){
		}
		const AbstractFunction* evaluate(RData& a) const override{
			registerFunctionPrototype(a);
			assert(myFunction);
			return myFunction;
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args) const override final{
			error("Function Declaration cannot be used as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			error("Function Declaration cannot be used as value");
			exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		const Token getToken() const override final{
			return T_FUNC;
		}
		E_FUNCTION* simplify() override final{
			return this;
		}
};



#endif /* E_FUNCTION_HPP_ */
