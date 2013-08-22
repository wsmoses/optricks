/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../constructs/Statement.hpp"

class E_PREOP : public Statement{
	public:
		const Token getToken() const override final{ return T_UOP; }
		Statement *value;
		String operation;
		E_PREOP(PositionID id, String o, Statement* a): Statement(id),
				value(a), operation(o)
		{};//possible refinement of return type
		Statement* simplify() override final {
			return new E_PREOP(filePos, operation,value->simplify());
		}
		Value* evaluate(RData& r) override final {
			Value* a = value->evaluate(r);
			return value->returnType->preops[operation]->apply(a,r);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
		ClassProto* checkTypes() override{
			value->checkTypes();
			auto found = value->returnType->preops.find(operation);
			if(found==value->returnType->preops.end())
				todo("Unary operator ",operation," not implemented for class ",
						value->returnType->name);
			return returnType = found->second->returnType;
		}
		void registerClasses(RData& r) override final{
			value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			value->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			value->resolvePointers();
		};
		FunctionProto* getFunctionProto() override final{ return NULL; }
};


class E_POSTOP : public Statement{
	public:
		const Token getToken() const override final{ return T_UOP; }
		Statement *value;
		String operation;
		E_POSTOP(PositionID id, String o, Statement* a): Statement(id),
				value(a), operation(o)
		{};
		Statement* simplify() override final {
			return new E_POSTOP(filePos, operation,value->simplify());
		}
		Value* evaluate(RData& r) override final {
			Value* a = value->evaluate(r);
			return value->returnType->postops[operation]->apply(a,r);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
		ClassProto* checkTypes() override{
			value->checkTypes();
			auto found = value->returnType->postops.find(operation);
			if(found==value->returnType->postops.end())
				todo("Unary operator ",operation," not implemented for class ",
						value->returnType->name);
			return returnType = found->second->returnType;
		}
		void registerClasses(RData& r) override final{
			value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			value->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			value->resolvePointers();
		};
		FunctionProto* getFunctionProto() override final{ return NULL; }
};

#endif /* E_BINOP_HPP_ */
