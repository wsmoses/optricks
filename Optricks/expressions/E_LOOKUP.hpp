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
		E_LOOKUP(PositionID id, String o, Statement* a, String b): Statement(id),
				left(a), right(b), operation(o){};//TODO allow more detail

		void write(ostream& f,String a="") const override{
			f << left;
			f << operation;
			f << right;
		}

		void registerClasses(RData& r) override final{
			todo("E_LOOKUP rC");
		}
		void registerFunctionArgs(RData& r) override final{
			todo("E_LOOKUP rFA");
		};
		void registerFunctionDefaultArgs() override final{
			todo("E_LOOKUP rFDA");
		};
		void resolvePointers() override final{
			todo("E_LOOKUP rP");
		}
		ClassProto* checkTypes(){
			todo("Check types for lookup");
		}
		Value* evaluate(RData& a) override{
			//TODO lookup variables
			todo("Variable lookup not implemented");
		}
		Statement* simplify() override{
			//TODO lookup variables
			return this;
		}
		FunctionProto* getFunctionProto() override final{
			error("E_LOOKUP getFunctionProto() not implemented");
			return NULL;
		}
};



#endif /* E_LOOKUP_HPP_ */
