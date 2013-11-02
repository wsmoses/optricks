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
		DATA llvmObject;
	public:
		funcMap funcs;
		ClassProto* getReturnType(RData& r) const{
			if(funcs.size()>0) return functionClass;
			else return llvmObject.getReturnType(r);
		}
		DATA getObject(PositionID id) const{
			if(funcs.size()>0){
				DATA d = funcs.get(id);
				return d;
			}
			return llvmObject;
		}
		void setObject(DATA d){
			llvmObject = d;
		}
		virtual ~ReferenceElement(){};
		ReferenceElement(String c, OModule* mod, String index, DATA value, funcMap fun):
			Resolvable(mod,index), llvmObject(value), funcs(fun)
		{
		}
		ReferenceElement(OModule* mod, String index, DATA value, ClassProto* cl, funcMap& fun):
			Resolvable(mod,index), llvmObject(value), funcs(fun)
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
	if(nam==name) id.error("Cannot make function with same name as class "+name);
	if(functions.find(nam)!=functions.end()) return functions[nam];
	if(hasInner(nam)) id.error("Cannot create another function type for class "+name+" named "+nam);
	return functions[nam] = new ReferenceElement("",NULL,name+"."+nam,DATA::getFunction(NULL,NULL),funcMap());
}

ClassProto* DATA::getReturnType(RData& r) const{
	if(type==R_GEN){
		assert(info.funcType!=NULL);
		return info.funcType->getGeneratorType(r);
	}
	if(type==R_FUNC) return functionClass;
	if(!(type==R_CONST || type==R_LOC || type==R_CLASS)){
		printf("You've run into a compiler error!\n");
		printf("Most likely this is because you tried to use a global variable in a function\n");
		printf("This is not yet supported but will be soon.\n");//TODO global variable in function
		//printf("Although you probably shouldn't be using globals");
	}
	assert(type==R_CONST || type==R_LOC || type==R_CLASS);
//	if(type!=R_CONST && type!=R_LOC && type!=R_CLASS) id.error("Could not get returnType of "+str<DataType>(type));
	assert(info.classType !=NULL);
	return info.classType;
}
#endif /* ALL_HPP_ */
