/*
 * E_SET.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_SET_HPP_
#define E_SET_HPP_

#include "../language/statement/Statement.hpp"
#include "./E_VAR.hpp"

class E_SET: public ErrorStatement{
	public:
		Statement* variable;
		Statement* value;
		virtual ~E_SET(){}
		E_SET(PositionID id, Statement* loc, Statement* e) : ErrorStatement(id){
			variable = loc;
			value = e;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
		const Token getToken() const final override{
			return T_SET;
		}
		const AbstractClass* getReturnType() const final override{
			auto rt = variable->getReturnType();
			assert(rt);
			assert(rt->classType!=CLASS_VOID);
			return rt;
		}

		void registerClasses() const override final{
			variable->registerClasses();
			value->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			variable->registerFunctionPrototype(r);
			value->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) const override final{
			variable->buildFunction(r);
			value->buildFunction(r);
		};
		const Data* evaluate(RData& r) const final override{
			auto to = variable->evaluate(r);
			if(to->type!=R_LOC){
				error("Cannot set a non-variable "+str(to->type));
				exit(1);
			}
			Location* aloc = ((LocationData*)to)->getMyLocation();
			Value* nex = value->evaluate(r)->castToV(r, to->getReturnType(), filePos);
			aloc->setValue(nex,r);
			return new ConstantData(nex,to->getReturnType());
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			return variable->getFunctionReturnType(id,args);
		}
};

#endif /* E_SET_HPP_ */
