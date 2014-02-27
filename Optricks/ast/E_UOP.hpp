/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../language/statement/Statement.hpp"

class E_UOP : public ErrorStatement{
public:
	Statement *value;
	String operation;
	virtual ~E_UOP(){};
	explicit E_UOP(PositionID id, String o, Statement* a): ErrorStatement(id),
			value(a), operation(o)
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
};
class E_PREOP : public E_UOP{
public:
	E_PREOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
	Statement* simplify() override final {
		return new E_PREOP(filePos, operation,value->simplify());
	}

	const AbstractClass* getReturnType() const override{
		return value->operationReturnType(filePos, "::"+operation, nullptr);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		const AbstractClass* t = value->operationReturnType(filePos, "::"+operation, nullptr);
		if(t->classType!=CLASS_FUNC)
			id.error("Class '"+t->getName()+"' cannot be used as function");
		FunctionClass* fc = (FunctionClass*)t;
		//todo add check for argument validation?
		return fc->returnType;
	}
	const Data* evaluate(RData& r) const override final {
		return value->evaluate(r)->applyOperation(r, filePos, "::"+operation, nullptr);
	}
	void write(ostream& f,String s="") const override{
		f << "(" << operation << value << ")";
	}
};


class E_POSTOP : public E_UOP{
public:
	E_POSTOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
	Statement* simplify() override final {
		return new E_POSTOP(filePos, operation,value->simplify());
	}
	const AbstractClass* getReturnType() const override{
		return value->operationReturnType(filePos, ":"+operation, nullptr);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		const AbstractClass* t = value->operationReturnType(filePos, "::"+operation, nullptr);
		if(t->classType!=CLASS_FUNC)
			id.error("Class '"+t->getName()+"' cannot be used as function");
		FunctionClass* fc = (FunctionClass*)t;
		//todo add check for argument validation?
		return fc->returnType;
	}
	const Data* evaluate(RData& r) const override final {
		return value->evaluate(r)->applyOperation(r, filePos, "::"+operation, nullptr);
	}
	void write(ostream& f,String s="") const override{
		f << "(" << value << " " << operation << ")";
	}
};

#endif /* E_BINOP_HPP_ */
