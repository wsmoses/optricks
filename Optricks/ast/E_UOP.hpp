/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../language/statement/Statement.hpp"
#include "../operators/Unary.hpp"
#include "../language/class/builtin/ReferenceClass.hpp"

enum UOP_TYPE{
	UOP_PRE,
	UOP_POST
};
class E_UOP : public ErrorStatement{
public:
	Statement *value;
	String operation;
	UOP_TYPE pre;
	virtual ~E_UOP(){};
	explicit E_UOP(PositionID id, String o, Statement* a, UOP_TYPE p): ErrorStatement(id),
			value(a), operation(o), pre(p)
	{};
	const Token getToken() const override final{ return T_UOP; }
	void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
	}
	void registerClasses() const override final{
		value->registerClasses();
	}
	void registerFunctionPrototype(RData& r) const override final{
		value->registerFunctionPrototype(r);
	};
	void buildFunction(RData& r) const override final{
		value->buildFunction(r);
	};
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		const AbstractClass* t = getReturnType();
		if(t->classType!=CLASS_FUNC)
			id.error("Class '"+t->getName()+"' cannot be used as function");
		FunctionClass* fc = (FunctionClass*)t;
		return fc->returnType;
	}
	const AbstractClass* getReturnType() const override{
		if(pre==UOP_PRE)
			return getPreopReturnType(filePos, value->getReturnType(), operation);
		else
			return getPostopReturnType(filePos, value->getReturnType(), operation);
	}
	const Data* evaluate(RData& r) const override final {
		if(pre==UOP_PRE)
			return getPreop(r, filePos, operation, value->evaluate(r));
		else
			return getPostop(r, filePos, operation, value->evaluate(r));
	}
};
#endif /* E_BINOP_HPP_ */
