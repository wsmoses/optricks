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
#include "Location.hpp"
#include "Scopable.hpp"

Value* ClassProto::generateData(RData& r){
	Type* t = getType(r);
	assert(t!=NULL);
	if(layoutType==POINTER_LAYOUT){
		uint64_t s = DataLayout(r.lmod->getDataLayout()).getTypeAllocSize(((PointerType*)t)->getElementType());
		Value* da = r.builder.CreateCall(o_malloc,getUInt32(s));//TODO decide malloc/calloc
		Value* dat = r.builder.CreatePointerCast(da,t);
		r.builder.CreateStore(getUInt32(0), r.builder.CreateConstGEP2_32(dat,0,0));
		return dat;
	} else
	return UndefValue::get(t);
}

DATA ouopNullCast::apply(DATA a, RData& m, PositionID id){
	return DATA::getConstant(ConstantPointerNull::get((PointerType*)(to->getType(m))),to);
}


ClassProto::ClassProto(ClassProto* sC, String n, Type* t,LayoutType pointer,bool isGe,bool allowsInne):
		Scopable(sC),
			allowsInner(allowsInne),type(t),
			localVariables(),
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
