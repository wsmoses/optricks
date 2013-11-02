/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../constructs/Statement.hpp"

class E_UOP : public Construct{
	public:
		Statement *value;
		String operation;
		virtual ~E_UOP(){};
		explicit E_UOP(PositionID id, String o, Statement* a): Construct(id,NULL),
				value(a), operation(o)
		{};
		const Token getToken() const override final{ return T_UOP; }
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		void registerClasses(RData& r) override final{
					value->registerClasses(r);
				}
				void registerFunctionPrototype(RData& r) override final{
					value->registerFunctionPrototype(r);
				};
				void buildFunction(RData& r) override final{
					value->buildFunction(r);
				};
};
class E_PREOP : public E_UOP{
	public:
		E_PREOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
		Statement* simplify() override final {
			return new E_PREOP(filePos, operation,value->simplify());
		}
		DATA evaluate(RData& r) override final {
			auto a = value->evaluate(r);
			return value->returnType->preops[operation]->apply(a,r,filePos);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
		ClassProto* checkTypes(RData& r) override{
			value->checkTypes(r);
			auto found = value->returnType->preops.find(operation);
			if(found==value->returnType->preops.end())
				error("Pre operator "+operation+" not implemented for class "+value->returnType->name);
			return returnType = found->second->returnType;
		}
};


class E_POSTOP : public E_UOP{
	public:
		E_POSTOP(PositionID id, String o, Statement* a):E_UOP(id,o,a){};
		Statement* simplify() override final {
			return new E_POSTOP(filePos, operation,value->simplify());
		}
		DATA evaluate(RData& r) override final {
			auto a = value->evaluate(r);
			return value->returnType->postops[operation]->apply(a,r,filePos);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << value << " " << operation << ")";
		}
		ClassProto* checkTypes(RData& r) override{
			value->checkTypes(r);
			auto found = value->returnType->postops.find(operation);
			if(found==value->returnType->postops.end())
				error("Post operator "+operation+" not implemented for class "+value->returnType->name);
			return returnType = found->second->returnType;
		}
};

#endif /* E_BINOP_HPP_ */
