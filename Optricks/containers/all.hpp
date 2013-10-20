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
	private:
	public:
		DATA llvmObject;
		ClassProto* returnClass;
		funcMap funcs;
		virtual ~ReferenceElement(){};
		ReferenceElement(String c, OModule* mod, String index, DATA value, ClassProto* cl, funcMap fun):
			Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun)
		{
		}
		ReferenceElement(OModule* mod, String index, DATA value, ClassProto* cl, funcMap& fun):
			Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun)
		{
		}
		ReferenceElement* resolve() override final{
			return this;
		}
		Value* getValue(RData& r/*,FunctionProto* func=NULL*/) const{
			return llvmObject.getValue(r);
		}
		//void ensureFunction(FunctionProto* func){

		//	}
		void setValue(DATA d, RData& r/*,FunctionProto* func=NULL*/){
			llvmObject.setValue(r,d.getValue(r));
		}
		void setValue(Value* d, RData& r/*,FunctionProto* func=NULL*/){
			llvmObject.setValue(r,d);
		}
};

ReferenceElement* ClassProto::addFunction(String nam, PositionID id){
	if(nam==name) todo("Cannot make function with same name as class "+name,id);
	if(innerDataIndex.find(nam)!=innerDataIndex.end()) todo("Cannot create another function type for class "+name+" named "+nam,id);
	if(functions.find(nam)==functions.end()) return functions[nam] = new ReferenceElement("",NULL,name+"."+nam,DATA::getNull(),functionClass,funcMap());
	return functions[nam];
}
#endif /* ALL_HPP_ */
