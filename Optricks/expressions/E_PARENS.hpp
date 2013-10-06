#ifndef E_PARENS_HPP_
#define E_PARENS_HPP_
#include "../constructs/Statement.hpp"

class E_PARENS : public Statement{
public:
	Statement* inner;
	virtual ~E_PARENS(){};
	E_PARENS(PositionID id, Statement* t) : Statement(id, t->returnType), inner(t) { };
	const Token getToken() const override{
		return T_PARENS;
	};
	ReferenceElement* getMetadata(RData& r) override final{
		return inner->getMetadata(r);
	}
	Value* getLocation(RData& a) override final {
		return inner->getLocation(a);
	}
	void registerClasses(RData& r) override final{
		inner->registerClasses(r);
	}
	ClassProto* getSelfClass() override final{
		return inner->getSelfClass();
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
	DATA evaluate(RData& a) override {
		return inner->evaluate(a);
	}
	Statement* simplify() override{
		return inner->simplify();
	}

	String getFullName() override final{
		return inner->getFullName();
	}
	void write (ostream& f,String b="") const override{
		f  << "(" << inner << ")";
	}
	ClassProto* checkTypes(RData& r) override final{
		return returnType = inner->checkTypes(r);
	}
};


#endif /* E_PARENS_HPP_ */
