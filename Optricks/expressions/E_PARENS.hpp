#ifndef E_PARENS_HPP_
#define E_PARENS_HPP_
#include "../constructs/Expression.hpp"

class E_PARENS : public Expression{
public:
	Expression* inner;
	E_PARENS(Expression* t) : Expression(t->returnType), inner(t) { };
	const Token getToken() const override{
		return T_PARENS;
	};
	Value* evaluate(RData& a) override {
		return inner->evaluate(a);
	}
	//TODO verify that this is valid and does not call order of ops again
	Expression* simplify() override{
		return inner->simplify();
	}
	void write (ostream& f,String b="") const override{
		f  << "(" << inner << ")";
	}
	void checkTypes(){
		inner->checkTypes();
		returnType = inner->returnType;
	}
};


#endif /* E_PARENS_HPP_ */
