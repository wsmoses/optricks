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
		ClassProto* getSelfClass(RData& r) override final{
			ClassProto* cp = pointer->resolve()->llvmObject.getMyClass(r);
			assert(cp!=NULL);
			return cp;
		}
		ReferenceElement* getMetadata(RData& r) final override{
			return pointer->resolve();
		}
		Statement* simplify() override final{
			return this;
		}
		DATA evaluate(RData& r) override final{
			return pointer->resolve()->llvmObject;
		}
		void write(ostream& f,String t="") const override{
			f  << pointer->name;
//			f << "v'" << pointer->name << "'";
		}

		void registerClasses(RData& r) override final{
			//TODO var register classes
		}
		void registerFunctionPrototype(RData& r) override final{
			//TODO var register function prototype
		};
		void buildFunction(RData& r) override final{
			//TODO var build function
		};
		ClassProto* checkTypes(RData& r) override{
			ClassProto* temp = pointer->resolve()->llvmObject.getReturnType(r);
			if(temp==NULL) error("Cannot determine return-type of variable "+pointer->name);
			return returnType = temp;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
};
#endif /* E_VAR_HPP_ */
