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
	if(toCast->classType==CLASS_VOID) return true;
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
	assert(hasCast(a));
	assert(hasCast(b));
	if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
	else if(a->classType==CLASS_VOID) return 1;
	else if(b->classType==CLASS_VOID) return -1;

	if(a->classType==b->classType){
		if(a->classType==CLASS_COMPLEX){
			ComplexClass* ca = (ComplexClass*)a;
			assert(ca->innerClass->classType==CLASS_FLOAT);
			ComplexClass* cb = (ComplexClass*)b;
			assert(cb->innerClass->classType==CLASS_FLOAT);
			FloatClass* fa = (FloatClass*)(ca->innerClass);
			FloatClass* fb = (FloatClass*)(cb->innerClass);
			if(fa->floatType==fb->floatType){
				if(fa==this)
					return (fb==this)?0:-1;
				else return (fb==this)?1:0;
			}
			else return (fa->floatType < fb->floatType)?(-1):(1);

		} else {
			FloatClass* fa = (FloatClass*)a;
			FloatClass* fb = (FloatClass*)b;
			if(fa->floatType==fb->floatType){
				if(fa==this)
					return (fb==this)?0:-1;
				else return (fb==this)?1:0;
			}
			else return (fa->floatType < fb->floatType)?(-1):(1);
		}
	} else return (a->classType==CLASS_FLOAT)?(-1):(1);
}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
llvm::Value* FloatClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		if(toCast->layout==LITERAL_LAYOUT) id.error("Cannot cast floating-point type "+getName()+" to "+toCast->getName());
		assert(valueToCast->getType()==type ||
				(valueToCast->getType()->isVectorTy() && valueToCast->getType()->getVectorElementType()==type));
		switch(toCast->classType){
		case CLASS_FLOAT:{
			auto ai = getWidth();
			auto bi = ((FloatClass*)toCast)->getWidth();
			if(ai>bi) id.error("Cannot cast floating-point type "+getName()+" to "+toCast->getName() +" floating type too small ("+str(ai)+","+str(bi)+")");
			if(ai==bi) return valueToCast;
			if(valueToCast->getType()->isVectorTy()){
				unsigned g = ((llvm::VectorType*)valueToCast->getType())->getVectorNumElements();
				auto TC = llvm::VectorType::get(toCast->type, g);
				return r.builder.CreateFPExt(valueToCast, TC);
			}
			return r.builder.CreateFPExt(valueToCast, toCast->type);
		}
		case CLASS_COMPLEX:{
			const RealClass* ac = ((const ComplexClass*)toCast)->innerClass;
			auto tmp = castTo(ac, r, id, valueToCast);
			auto v = llvm::ConstantVector::getSplat(2, ac->getZero(id));
			return r.builder.CreateInsertElement(v,tmp,getInt32(0));
		}
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
