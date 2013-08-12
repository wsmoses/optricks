/*
 * E_EXT.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_EXT_HPP_
#define E_EXT_HPP_
#include "../constructs/Expression.hpp"

class E_EXT : public Statement{
public:
	String value;
	E_EXT(String a){value = a;}
	const Token getToken() const override{
		return T_EXT;
	}
	void write(ostream& f,String a) const override{
		f << "extern " << value;
	}

	void* evaluate(Jump& jump) override{
		//TODO implement
		cerr << "Extern not implemented";
		exit(0);
		return VOID;
	}
	Statement* simplify(Jump& jump) override{
		return this;
	}
};

#endif /* E_EXT_HPP_ */
