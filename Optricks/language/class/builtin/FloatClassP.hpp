/*
 * FloatClass.cpp
 *
 *  Created on: Jan 3, 2014
 *      Author: Billy
 */
#ifndef FLOATCLASSP_HPP_
#define FLOATCLASSP_HPP_
#include "FloatClass.hpp"
#include "ComplexClass.hpp"
#include "../../RData.hpp"
bool FloatClass::hasCast(const AbstractClass* const toCast) const {
		if(toCast->layout!=PRIMITIVE_LAYOUT) return false;
		switch(toCast->classType){
		case CLASS_FLOAT:{
			return ((FloatClass*)toCast)->getWidth() >= getWidth();
		}
		case CLASS_COMPLEX:
			return ((ComplexClass*)toCast)->innerClass->classType==CLASS_FLOAT
					&& ((FloatClass*)(((ComplexClass*)toCast)->innerClass))->getWidth()>=getWidth();
		case CLASS_RATIONAL:
			assert(0);
			return false;
		default:
			return false;
		}
	}
int FloatClass::compare(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(a->classType==CLASS_COMPLEX || a->classType==CLASS_FLOAT);
	assert(b->classType==CLASS_COMPLEX || b->classType==CLASS_FLOAT);
	if(a->classType==b->classType){
		if(a->classType==CLASS_COMPLEX){
			ComplexClass* ca = (ComplexClass*)a;
			assert(ca->innerClass->classType==CLASS_FLOAT);
			ComplexClass* cb = (ComplexClass*)b;
			assert(cb->innerClass->classType==CLASS_FLOAT);
			FloatClass* fa = (FloatClass*)(ca->innerClass);
			FloatClass* fb = (FloatClass*)(cb->innerClass);
			if(fa->floatType==fb->floatType) return 0;
			else return (fa->floatType < fb->floatType)?(-1):(1);

		} else {
			FloatClass* fa = (FloatClass*)a;
			FloatClass* fb = (FloatClass*)b;
			if(fa->floatType==fb->floatType) return 0;
			else return (fa->floatType < fb->floatType)?(-1):(1);
		}
	} else return (a->classType==CLASS_FLOAT)?(-1):(1);
}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* FloatClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		if(toCast->layout!=LITERAL_LAYOUT) id.error("Cannot cast floating-point type "+getName()+" to "+toCast->getName());
		switch(toCast->classType){
		case CLASS_FLOAT:{
			if( ((FloatClass*)toCast)->getWidth() < getWidth()) id.error("Cannot cast floating-point type "+getName()+" to "+toCast->getName());
			return r.builder.CreateFPExt(valueToCast, toCast->type);
		}
		case CLASS_COMPLEX:
			if(((ComplexClass*)toCast)->innerClass->classType!=CLASS_FLOAT
					|| ((FloatClass*)(((ComplexClass*)toCast)->innerClass))->getWidth()<getWidth())
			id.error("Cannot cast floating-point type "+getName()+" to "+toCast->getName());
			return r.builder.CreateInsertElement(ConstantVector::getSplat(2, getZero(id)), valueToCast, getInt32(0));
		case CLASS_RATIONAL:
			id.error("Does not exist");
			assert(0);
			exit(1);
		default:
			id.error("Does not exist");
			assert(0);
			exit(1);
		}
		id.error("Internal Compiler Error - Cannot castTo of LiteralClass -- todo not luterak");
		exit(1);
	}


#endif
