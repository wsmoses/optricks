/*
 * E_VAR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_VAR_HPP_
#define E_VAR_HPP_

#include "../language/statement/Statement.hpp"

class E_VAR : public VariableReference {
	public:
		const Resolvable pointer;
		virtual ~E_VAR(){};
		E_VAR(const Resolvable& a) : VariableReference(),pointer(a){};
//		E_VAR(PositionID id, Resolvable& a) : VariableReference(id),pointer(a){};
		String getFullName() {
			return pointer.module->getScopeName()+"."+pointer.name;
		}
		String getShortName() override final{
			return pointer.name;
		}
		const Token getToken() const override{
			return T_VAR;
		}
		Resolvable getMetadata()
		//final override
		{
			return pointer;
		}
		const Data* evaluate(RData& r) const override final{
			auto tmp =  pointer.getObject();
			assert(tmp);
			return tmp;
		}
		void registerClasses() const override final{
			//TODO force var register classes
		}
		void registerFunctionPrototype(RData& r) const override final{
			//TODO force var register function prototype
		};
		void buildFunction(RData& r) const override final{
			//TODO force var build function
		};
		const AbstractClass* getReturnType() const override{
			const AbstractClass* temp = pointer.getReturnType();
			if(temp==NULL) pointer.filePos.error("Cannot determine return-type of variable "+pointer.name);
			return temp;
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
			return pointer.getFunctionReturnType(args);
		}

		AbstractClass* getSelfClass(PositionID id) override final{
			return pointer.getClass();
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
};
#endif /* E_VAR_HPP_ */
