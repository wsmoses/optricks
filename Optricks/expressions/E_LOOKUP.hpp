/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../constructs/Expression.hpp"

class E_LOOKUP : public Expression{
	public:
		const Token getToken() const override{ return T_LOOKUP; }
		Expression* left;
		String right;
		String operation;
		E_LOOKUP(String o, Expression* a, String b): Expression(objectClass),
				left(a), right(b), operation(o){};//TODO allow more detail

		void write(ostream& f,String a="") const override{
			f << left;
			f << operation;
			f << right;
		}
		void checkTypes(){
			todo("Check types for lookup");
		}
		Value* evaluate(RData& a,LLVMContext& context) override{
			//TODO lookup variables
			todo("Variable lookup not implemented");
		}
		Expression* simplify() override{
			//TODO lookup variables
			return this;
		}
};



#endif /* E_LOOKUP_HPP_ */
