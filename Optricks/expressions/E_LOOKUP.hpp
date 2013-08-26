/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../constructs/Statement.hpp"

class E_LOOKUP : public Statement{
	public:
		const Token getToken() const override{ return T_LOOKUP; }
		Statement* left;
		String right;
		String operation;
		E_LOOKUP(PositionID id, Statement* a,  String b, String o): Statement(id),
				left(a), right(b), operation(o){};//TODO allow more detail

		void write(ostream& f,String a="") const override{
			f << left;
			f << operation;
			f << right;
		}

		void registerClasses(RData& r) override final{
			error("E_LOOKUP rC");
		}
		void registerFunctionArgs(RData& r) override final{
			error("E_LOOKUP rFA");
		};
		void registerFunctionDefaultArgs() override final{
			error("E_LOOKUP rFDA");
		};
		void resolvePointers() override final{
			error("E_LOOKUP rP");
		}
		ClassProto* checkTypes(){
			error("Check types for lookup");
		}
		Value* evaluate(RData& a) override{
			//TODO lookup variables
			error("Variable lookup not implemented");
		}
		Statement* simplify() override{
			//TODO lookup variables
			return this;
		}

		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); }
};



#endif /* E_LOOKUP_HPP_ */
