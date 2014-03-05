/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../language/statement/Statement.hpp"
#include "../language/class/builtin/ReferenceClass.hpp"
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
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		const AbstractClass* t = getReturnType();
		if(t->classType!=CLASS_FUNC)
			id.error("Class '"+t->getName()+"' cannot be used as function");
		FunctionClass* fc = (FunctionClass*)t;
		return fc->returnType;
	}
};
class E_PREOP : public E_UOP{
public:
	E_PREOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
	const AbstractClass* getReturnType() const override{
		const AbstractClass* const cc = value->getReturnType();
		if(operation=="&"){
			if(cc->classType==CLASS_REF) filePos.error("Cannot get reference of reference");
			if(cc->classType==CLASS_LAZY) filePos.error("Cannot get reference of lazy");
			else return ReferenceClass::get(cc);
		}
		return cc->getLocalFunction(filePos,":"+operation,{value})->getSingleProto()->returnType;
	}
	const Data* evaluate(RData& r) const override final {
		const AbstractClass* const cc = value->getReturnType();
		if(operation=="&"){
			if(cc->classType==CLASS_REF) filePos.error("Cannot get reference of reference");
			if(cc->classType==CLASS_LAZY) filePos.error("Cannot get reference of lazy");
			else{
				filePos.compilerError("Allow creating references");
				exit(1);
			}
		}
		return cc->getLocalFunction(filePos,":"+operation,{value})->callFunction(r,filePos,{value});
	}
};


class E_POSTOP : public E_UOP{
public:
	E_POSTOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
	const AbstractClass* getReturnType() const override{
		const AbstractClass* const cc = value->getReturnType();
		return cc->getLocalFunction(filePos,":"+operation,{value,nullptr})->getSingleProto()->returnType;
	}
	const Data* evaluate(RData& r) const override final {
		const AbstractClass* const cc = value->getReturnType();
		//todo check if second should have null
		return cc->getLocalFunction(filePos,":"+operation,{value,nullptr})->callFunction(r,filePos,{value});
	}
};

#endif /* E_BINOP_HPP_ */
