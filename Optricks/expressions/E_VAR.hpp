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
		virtual ~E_VAR(){};
		E_VAR(PositionID id, Resolvable* a) : Statement(id),pointer(a){};
		String getFullName() override final{
			return pointer->name;
		}
		const Token getToken() const override{
			return T_VAR;
		}
		ReferenceElement* getMetadata(RData& r) final override{
			return pointer->resolve();
		}
		Statement* simplify() override final{
			return this;
		}
		DATA evaluate(RData& r) override final{
			return pointer->resolve()->getObject(filePos);
		}
		void write(ostream& f,String t="") const override{
			f  << pointer->name;
//			f << "v'" << pointer->name << "'";
		}

		void registerClasses(RData& r) override final{
			//TODO force var register classes
		}
		void registerFunctionPrototype(RData& r) override final{
			//TODO force var register function prototype
		};
		void buildFunction(RData& r) override final{
			//TODO force var build function
		};
		ClassProto* checkTypes(RData& r) override{
			ClassProto* temp = pointer->resolve()->getReturnType(r);
			if(temp==NULL) error("Cannot determine return-type of variable "+pointer->name);
			return returnType = temp;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
};
#endif /* E_VAR_HPP_ */
