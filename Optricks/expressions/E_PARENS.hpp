#ifndef E_PARENS_HPP_
#define E_PARENS_HPP_
#include "../constructs/Statement.hpp"

class E_PARENS : public Statement{
public:
	Statement* inner;
	E_PARENS(PositionID id, Statement* t) : Statement(id, t->returnType), inner(t) { };
	const Token getToken() const override{
		return T_PARENS;
	};

	AllocaInst* getAlloc() override final{ return inner->getAlloc(); };
	FunctionProto* getFunctionProto() override final{ return inner->getFunctionProto(); }
	void registerClasses(RData& r) override final{
		inner->registerClasses(r);
	}
	void registerFunctionArgs(RData& r) override final{
		inner->registerFunctionArgs(r);
	}
	void registerFunctionDefaultArgs() override final{
		inner->registerFunctionDefaultArgs();
	}
	void resolvePointers() override final{
		inner->resolvePointers();
	}
	Value* evaluate(RData& a) override {
		return inner->evaluate(a);
	}
	//TODO verify that this is valid and does not call order of ops again
	Statement* simplify() override{
		return inner->simplify();
	}
	void write (ostream& f,String b="") const override{
		f  << "(" << inner << ")";
	}
	ClassProto* checkTypes() override final{
		return returnType = inner->checkTypes();
	}
};


#endif /* E_PARENS_HPP_ */
