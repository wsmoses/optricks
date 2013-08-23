/*
 * E_VAR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_VAR_HPP_
#define E_VAR_HPP_

#include "../constructs/Statement.hpp"
#include "../constructs/Module.hpp"

class E_VAR : public Statement{
public:
	Resolvable* pointer;
//	Resolvable* type;
	E_VAR(PositionID id, Resolvable* a) : Statement(id),pointer(a){
	//	type = ty;
	};
	const Token getToken() const override{
		return T_VAR;
	}
	Statement* simplify() override final{
		return this;
	}
	Value* evaluate(RData& r) override final{
		//TODO variables not implemented
		Value* ans = pointer->resolve();
		if(pointer->resolveAlloc()!=NULL) return r.builder.CreateLoad(pointer->resolveAlloc());
		if(ans==NULL){
			error("Could not resolve pointer "+pointer->name);
		}
		return ans;
	}
	void write(ostream& f,String t="") const override{
		f << "E_VAR('" << pointer->name << "')";
	}

	void registerClasses(RData& r) override final{
	//TODO
	}
	void registerFunctionArgs(RData& r) override final{
		//TODO
	};
	void registerFunctionDefaultArgs() override final{
		//TODO
	};
	ClassProto* checkTypes() override{
		ClassProto* temp = pointer->resolveReturnClass();
		if(temp==NULL) error("Cannot determine return-type of variable "+pointer->name);
		return returnType = temp;
	}
	void resolvePointers() override{
		LateResolve* r = dynamic_cast<LateResolve*>(pointer);
		if(r!=NULL){
			pointer = r->resolvePointer();
		}
		if(pointer == NULL) error("What?? Pointer is null? for " + pointer->name);
	}
	FunctionProto* getFunctionProto() override final{ return pointer->resolveFunction(); }
	AllocaInst* getAlloc() override final{ return pointer->resolveAlloc(); };
};
#endif /* E_VAR_HPP_ */
