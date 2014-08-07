/*
 * E_FUNCTION.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef E_FUNCTION_HPP_
#define E_FUNCTION_HPP_
#include "../../language/statement/Statement.hpp"
class E_FUNCTION:public ErrorStatement{
	public:
		mutable OModule module;
		std::vector<Declaration*> declaration;
		mutable SingleFunction* myFunction;
		String name;
		Statement* methodBody;
		Statement* returnV;
		E_FUNCTION(PositionID id, const OModule& myMod,String nam):
			ErrorStatement(id),module(myMod),declaration(),myFunction(nullptr),name(nam){
			methodBody = nullptr;
			returnV = nullptr;
			//todo remove
		}
		void reset() const override final{}
		const AbstractClass* getMyClass(RData& r, PositionID id)const{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		const AbstractFunction* evaluate(RData& a) const override{
			registerFunctionPrototype(a);
			assert(myFunction);
			assert(myFunction->getReturnType());
			return myFunction;
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod) const override final{
			if(!myFunction) registerFunctionPrototype(rdata);
			return myFunction->getSingleProto()->returnType;
		}
		const AbstractClass* getReturnType() const override final{
			if(!myFunction) registerFunctionPrototype(rdata);
			return myFunction->getSingleProto()->getFunctionClass();
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		const Token getToken() const override{
			return T_FUNC;
		}
};



#endif /* E_FUNCTION_HPP_ */
