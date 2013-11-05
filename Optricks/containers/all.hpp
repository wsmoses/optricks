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
#include "Tuple.hpp"
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
#ifndef NDEBUG
	if(!(type==R_CONST || type==R_LOC || type==R_CLASS)){
		cerr << "Error " << str<DataType>(type) << endl << flush;
		assert(0);
	}

#endif
//	if(type!=R_CONST && type!=R_LOC && type!=R_CLASS) id.error("Could not get returnType of "+str<DataType>(type));
	assert(info.classType !=NULL);
	return info.classType;
}

DATA ClassProto::generateData(RData& r){
	Type* t = getType(r);
	assert(t!=NULL);
	if(layoutType==POINTER_LAYOUT){
		uint64_t s = DataLayout(r.lmod->getDataLayout()).getTypeAllocSize(((PointerType*)t)->getElementType());
		Value* da = r.builder.CreateCall(o_malloc,getUInt32(s));//TODO decide malloc/calloc
		Value* dat = r.builder.CreatePointerCast(da,t);
		r.builder.CreateStore(getUInt32(0), r.builder.CreateConstGEP2_32(dat,0,0));
		return DATA::getConstant(dat,this);
	} else
	return DATA::getConstant(UndefValue::get(t), this);
}

DATA ouopNullCast::apply(DATA a, RData& m, PositionID id){
	return DATA::getConstant(ConstantPointerNull::get((PointerType*)(to->getType(m))),to);
}


ClassProto::ClassProto(ClassProto* sC, String n, Type* t,LayoutType pointer,bool isGe,bool allowsInne):
			allowsInner(allowsInne),type(t),
			innerDataIndex((sC==NULL)?(std::map<String, unsigned int>()):(sC->innerDataIndex)),

			innerData((sC==NULL)?(std::vector<ClassProto*>()):(sC->innerData)),
			functions((sC==NULL)?(std::map<String, ReferenceElement* >()):(sC->functions)),
			superClass(sC),
			constructors(),
			layoutType(pointer),isGen(isGe),
			name(n)
		{
			casts.insert(std::pair<ClassProto*, ouop*>(this,new ouopNative([](DATA a, RData& m, PositionID id) -> DATA{	return a; }
			, this)));
			if((layoutType==POINTER_LAYOUT || layoutType==PRIMITIVEPOINTER_LAYOUT) && n!="`NullClass"){
				assert(nullClass);
				addBinop("==",nullClass) = NULLCHECK1;
				addBinop("!=",nullClass) = NULLCHECK2;
				nullClass->addBinop("==",this) = NULLCHECK3;
				nullClass->addBinop("!=",this) = NULLCHECK4;
			}
		}


Type* ClassProto::getType(RData& r){
	if(type!=NULL) return type;
	else{
		// if(innerData.size()==0 && !isGen) cerr << "what.. " << name << endl << flush;
		StructType* structType = StructType::create(r.lmod->getContext(), name);
		if(layoutType==POINTER_LAYOUT){
			type = PointerType::getUnqual(structType);
		} else{
			type = structType;
		}
		std::vector<Type*> types;
		addTypes(types,r);
		structType->setBody(ArrayRef<Type*>(types),false);
		assert(type!=NULL);
		return type;
	}
}
#endif /* ALL_HPP_ */
