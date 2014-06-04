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

	const AbstractClass* getMyClass(RData& r, PositionID id, const std::vector<TemplateArg>& args)const{
		if(args.size()!=0){
			id.compilerError("Template error");
		}
		auto t = value->getMyClass(r, id, {});
		if(pre==UOP_POST){
			if(operation=="[]") return ArrayClass::get(t,0);
			if(operation=="&") return ReferenceClass::get(t);
			if(operation=="%") return LazyClass::get(t);
		}
		id.compilerError("Cannot getSelfClass of OUOP "+operation);
		exit(1);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		assert(isClassMethod==false);
		const AbstractClass* ac = getReturnType();
		if(ac->classType==CLASS_FUNC){
			return ((FunctionClass*)ac)->returnType;
		} else if(ac->classType==CLASS_LAZY){
			return ((LazyClass*)ac)->innerType;
		} else if(ac->classType==CLASS_CLASS){
			return ac;
		}	else {
			id.error("Class '"+ac->getName()+"' cannot be used as function");
			exit(1);
		}
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
