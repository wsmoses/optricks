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
	String name;
	E_VAR(String a) : Expression(objectClass){name = a; };
	const Token getToken() const override{
		return T_VAR;
	}
	Expression* simplify(){
		return this;
	}
	oobject* evaluate(){
		//TODO variables not implemented
		cerr << "Variables not implemented" << endl;
		exit(0);
		return NULL;
	}
	void write(ostream& f,String t="") const override{
		f << "E_VAR('" << name << "')";
	}
};
#endif /* E_VAR_HPP_ */
