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
		Resolvable pointer;
		T_ARGS t_args;
		virtual ~E_VAR(){};
		E_VAR(const Resolvable& a, bool isTemplate) : VariableReference(),pointer(a), t_args(isTemplate){
			assert(pointer.module);
		};
//		E_VAR(PositionID id, Resolvable& a) : VariableReference(id),pointer(a){};
		String getFullName() const {
			return pointer.module->getScopeName()+"."+pointer.name;
		}
		String getShortName() const override final{
			return pointer.name;
		}
		const Token getToken() const override{
			return T_VAR;
		}

		void reset() const override final{
		}
		const Resolvable getMetadata() const
		//final override
		{
			assert(pointer.module);
			return pointer;
		}
		const Data* evaluate(RData& r) const override final{
			assert(pointer.module);
			/*if(t_args.inUse){
				pointer.filePos.warning("Using only class templates");
				return pointer.getClass(t_args);
			}*/
			auto tmp = pointer.getObject(r, t_args);
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
			const AbstractClass* temp = pointer.getReturnType(t_args);
			if(temp==NULL) pointer.filePos.error("Cannot determine return-type of variable "+pointer.name);
			return temp;
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			assert(isClassMethod==false);
			return pointer.getFunctionReturnType(t_args, args);
		}

		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			return pointer.getClass(t_args);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
};
#endif /* E_VAR_HPP_ */
