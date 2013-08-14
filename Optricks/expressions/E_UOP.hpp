/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../constructs/Expression.hpp"

class E_PREOP : public Expression{
	public:
		const Token getToken() const override final{ return T_UOP; }
		Expression *value;
		String operation;
		E_PREOP(String o, Expression* a): Expression(objectClass),
				value(a), operation(o)
		{};//possible refinement of return type
		Expression* simplify() override final {
			return new E_PREOP(operation,value->simplify());
		}
		Value* evaluate(RData& r,LLVMContext& context) override final {
			Value* a = value->evaluate(r,context);
			return value->returnType->preops[operation]->apply(a,r);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
		void checkTypes(){
			value->checkTypes();
			auto found = value->returnType->preops.find(operation);
			if(found==value->returnType->preops.end())
						todo("Unary operator ",operation," not implemented for class ",
								value->returnType->name);
			returnType = found->second->returnType;
		}
};


class E_POSTOP : public Expression{
	public:
		const Token getToken() const override final{ return T_UOP; }
		Expression *value;
		String operation;
		E_POSTOP(String o, Expression* a): Expression(objectClass),
				value(a), operation(o)
		{};
		Expression* simplify() override final {
			return new E_POSTOP(operation,value->simplify());
		}
		Value* evaluate(RData& r,LLVMContext& context) override final {
			Value* a = value->evaluate(r,context);
			return value->returnType->postops[operation]->apply(a,r);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
		void checkTypes(){
			value->checkTypes();
			auto found = value->returnType->postops.find(operation);
			if(found==value->returnType->postops.end())
						todo("Unary operator ",operation," not implemented for class ",
								value->returnType->name);
			returnType = found->second->returnType;
		}
};

#endif /* E_BINOP_HPP_ */
