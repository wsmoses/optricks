/*
 * all.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef ALL_HPP_
#define ALL_HPP_

#include "settings.hpp"
#include "ClassProto.hpp"
#include "FunctionProto.hpp"
#include "GenericClass.hpp"
#include "operations.hpp"
#include "RData.hpp"
#include "types.hpp"
#include "operators.hpp"
#include "basic_functions.hpp"

#ifndef REFERENCEELEM_C_
#define REFERENCEELEM_C_
class ReferenceElement;
#endif
class Resolvable{
	public:
	OModule* module;
	String name;
	Resolvable(OModule* m, String n):module(m),name(n){};
	virtual ~Resolvable(){};
	virtual ReferenceElement* resolve() = 0;
};


#define REFERENCEELEM_C_
class ReferenceElement:public Resolvable{
public:
	DATA llvmObject;
	ClassProto* returnClass;
	FunctionProto* function;
	ClassProto* selfClass;
	Value* llvmLocation;
	virtual ~ReferenceElement(){};
	ReferenceElement(OModule* mod, String index, DATA value, ClassProto* cl, FunctionProto* fun, ClassProto* selfCl,Value* al=NULL):
		Resolvable(mod,index), llvmObject(value), returnClass(cl), function(fun), selfClass(selfCl), llvmLocation(al)
	{
	}
	ReferenceElement* resolve() override final;
	DATA getValue(RData& r){
		if(llvmLocation==NULL) return llvmObject;
		else return r.builder.CreateLoad(llvmLocation);
	}
	DATA setValue(DATA d, RData& r){
		if(llvmLocation==NULL) return llvmObject=d;
		else return r.builder.CreateStore(d,llvmLocation);
	}
};
#endif /* ALL_HPP_ */
