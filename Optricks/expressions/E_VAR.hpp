/*
 * E_VAR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_VAR_HPP_
#define E_VAR_HPP_

#include "../constructs/Expression.hpp"

class E_VAR : public Expression{
public:
	Resolvable* pointer;
	E_VAR(Resolvable* a) : Expression(objectClass),pointer(a){ };
	const Token getToken() const override{
		return T_VAR;
	}
	Expression* simplify() override final{
		return this;
	}
	Value* evaluate(RData& a) override final{
		//TODO variables not implemented
		Value* ans = pointer->resolve();
		if(ans==NULL) todo("Could not resolve pointer "+pointer->name);
		return ans;
	}
	void write(ostream& f,String t="") const override{
		f << "E_VAR('" << pointer->name << "')";
	}
	void checkTypes(){
		//todo("E_VAR Check types not implemented: "+pointer->name);
	}
};
#endif /* E_VAR_HPP_ */
