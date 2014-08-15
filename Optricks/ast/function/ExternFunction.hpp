/*
 * ExternFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef EXTERNFUNCTION_HPP_
#define EXTERNFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../language/basic_functions.h"
class ExternFunction : public E_FUNCTION{
public:
	ExternFunction(PositionID id, OModule* o,String n):
		E_FUNCTION(id,OModule(o),n){
		returnV=nullptr;
	}
	void reset() const override final{
		myFunction = nullptr;
	}
	void registerClasses() const override final{
		//returnV->registerClasses();
	}
	void buildFunction(RData& a) const override final{
		registerFunctionPrototype(a);
		for(auto& d: declaration) d->buildFunction(a);
		assert(methodBody==nullptr);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		assert(methodBody==nullptr);
		//returnV->registerFunctionPrototype(a);
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			assert(ac);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			assert(ac->type);
		}
		assert(returnV);
		const AbstractClass* returnType = returnV->getMyClass(filePos);
		myFunction = new ExternalFunction(new FunctionProto(name, ad, returnType), ""/*lib*/);
		module.surroundingScope->addFunction(filePos, name)->add(myFunction, filePos);
		for(auto& d: declaration) d->registerFunctionPrototype(a);
	}
};



#endif /* EXTERNFUNCTION_HPP_ */
