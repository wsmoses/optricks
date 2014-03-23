/*
 * IntClass.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */
#ifndef INTCLASSP_HPP_
#define INTCLASSP_HPP_
#include "IntClass.hpp"
#include "FloatClass.hpp"
#include "ComplexClass.hpp"
#include "../../RData.hpp"

inline Value* IntClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
	if(toCast->layout==LITERAL_LAYOUT) id.error("Cannot cast integer type to "+toCast->getName());
	switch(toCast->classType){
	case CLASS_INT:{
		IntClass* nex = (IntClass*)toCast;
		auto n_width = nex->getWidth();
		auto s_width = getWidth();
		if(n_width>s_width){
			return r.builder.CreateSExt(valueToCast,nex->type);
		}
		else if(n_width<s_width) id.error("Cannot cast integer type of width "+str(getWidth())+" to integer type of width"+str(nex->getWidth()));
		assert(n_width==s_width);
		return valueToCast;
		//}
	}
	case CLASS_FLOAT:{
		assert(((FloatClass*)toCast)->type);
		return r.builder.CreateSIToFP(valueToCast, toCast->type);
	}
	//case CLASS_RATIONAL:
	case CLASS_COMPLEX:{
		const RealClass* ac = ((const ComplexClass*)toCast)->innerClass;
		Value* tmp = castTo(ac, r, id, valueToCast);
		auto v = ConstantVector::getSplat(2, ac->getZero(id));
		return r.builder.CreateInsertElement(v,tmp,getInt32(0));
	}
	default:
		id.error("Cannot cast integer type ("+getName()+") to "+toCast->getName());
		exit(1);
	}
}

//todo allow rationals
int IntClass::compare(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(hasCast(a));
	assert(hasCast(b));
	if(a->classType==b->classType){
		if(a->classType==CLASS_COMPLEX){
			ComplexClass* ca = (ComplexClass*)a;
			assert(ca->innerClass->classType==CLASS_FLOAT || ca->innerClass->classType==CLASS_INT);
			ComplexClass* cb = (ComplexClass*)b;
			assert(cb->innerClass->classType==CLASS_FLOAT || cb->innerClass->classType==CLASS_INT);
			if(ca->innerClass->classType==cb->innerClass->classType){
				if(ca->innerClass->classType==CLASS_FLOAT){
					return 0;
					/*
					FloatClass* fa = (FloatClass*)(ca->innerClass);
					FloatClass* fb = (FloatClass*)(cb->innerClass);
					if(fa->floatType==fb->floatType) return 0;
					else return (fa->floatType < fb->floatType)?(-1):(1);
					*/
				} else {
					IntClass* ia = (IntClass*)(ca->innerClass);
					IntClass* ib = (IntClass*)(cb->innerClass);

					if(ia->getWidth()==ib->getWidth()) return 0;
					else return (ia->getWidth() < ib->getWidth())?(-1):(1);
				}
			} else {
				return (ca->innerClass->classType==CLASS_INT)?(-1):(1);
			}

		} else if(a->classType==CLASS_FLOAT){
			return 0;
			/*FloatClass* fa = (FloatClass*)a;
			FloatClass* fb = (FloatClass*)b;
			if(fa->floatType==fb->floatType) return 0;
			else return (fa->floatType < fb->floatType)?(-1):(1);*/
		} else {
			IntClass* ia = (IntClass*)(a);
			IntClass* ib = (IntClass*)(b);
			if(ia->getWidth()==ib->getWidth()) return 0;
			else return (ia->getWidth() < ib->getWidth())?(-1):(1);
		}
	} else{
		return (a->classType==CLASS_INT)?(-1):(
				(b->classType==CLASS_INT)?(1):(
						(a->classType==CLASS_FLOAT)?(-1):(1)
				)
				);
	}
}
#endif
