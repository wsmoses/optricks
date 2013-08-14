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

	Value* evaluate(RData& a,LLVMContext& context) override{
		//TODO implement
		todo("Extern not implemented");
	}
	Statement* simplify(Jump& jump) override{
		return this;
	}
	void checkTypes(){
		todo("Check types of extern");
	}
};

#endif /* E_EXT_HPP_ */
