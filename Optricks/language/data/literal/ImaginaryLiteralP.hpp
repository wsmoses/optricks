/*
 * ComplexLiteralP.hpp
 *
 *  Created on: Jan 4, 2014
 *      Author: Billy
 */

#ifndef COMPLEXLITERALP_HPP_
#define COMPLEXLITERALP_HPP_

#include "ImaginaryLiteral.hpp"
#include "../../class/builtin/IntClass.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/builtin/ComplexClass.hpp"
#include "../../class/builtin/RationalClass.hpp"

const AbstractClass* ImaginaryLiteral::getReturnType() const{
	const AbstractClass* ac = imag->getReturnType();
	assert(ac->classType==CLASS_INTLITERAL || ac->classType==CLASS_FLOATLITERAL);
	return ComplexClass::get((const RealClass*)(ac));
}

ConstantVector* ImaginaryLiteral::getValue(RData& r, PositionID id) const{
	id.error("Cannot getValue of complex class without type");
	exit(1);
}

const Data* ImaginaryLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	if(right->classType!=CLASS_COMPLEX){
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
	const ComplexClass* cc = (const ComplexClass*)right;
	switch(cc->innerClass->classType){
	case CLASS_FLOATLITERAL:
	case CLASS_INTLITERAL:{
		auto im = imag->castTo(r, right, id);
		if(im==imag) return this;
		else return new ImaginaryLiteral(im);
	}
	case CLASS_FLOAT:
	case CLASS_INT:{
		Value* nimag = imag->castToV(r, cc->innerClass, id);
		if(Constant* d = dyn_cast<Constant>(nimag)){
			SmallVector<Constant*,2> ar(2);
			ar[0] = cc->innerClass->getZero(id);
			ar[1] = d;
			return new ConstantData(ConstantVector::get(ar),cc);
		}
		else{
			return new ConstantData(r.builder.CreateInsertElement(ConstantVector::getSplat(2, cc->innerClass->getZero(id))
				, nimag, getInt32(1)), cc);
		}
	}
	default:
		id.compilerError("Found unknown type as argument of complex");
		exit(1);
	}
}

Value* ImaginaryLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	if(right->classType!=CLASS_COMPLEX){
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
	const ComplexClass* cc = (const ComplexClass*)right;
	switch(cc->innerClass->classType){
	case CLASS_FLOAT:
	case CLASS_INT:{
		Value* nimag = imag->castToV(r, cc->innerClass, id);
		if(Constant* d = dyn_cast<Constant>(nimag)){
			SmallVector<Constant*,2> ar(2);
			ar[0] = cc->innerClass->getZero(id);
			ar[1] = d;
			return ConstantVector::get(ar);
		}
		else{
			Constant* cv = ConstantVector::getSplat(2, cc->innerClass->getZero(id));
			return r.builder.CreateInsertElement(cv, nimag, getInt32(1));
		}
	}
	default:
		id.compilerError("Found unknown type as argument of complex");
		exit(1);
	}
}

bool ImaginaryLiteral::hasCastValue(const AbstractClass* const a) const{
	if(a->classType!=CLASS_COMPLEX) return false;
	const ComplexClass* cc = (const ComplexClass*)a;
	return imag->hasCastValue(cc->innerClass);
}

int ImaginaryLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(a->classType==CLASS_COMPLEX);
	assert(b->classType==CLASS_COMPLEX);
	const ComplexClass* ca = (const ComplexClass*)a;
	const ComplexClass* cb = (const ComplexClass*)a;
	return imag->compareValue(ca->innerClass, cb->innerClass);
}

#endif /* COMPLEXLITERALP_HPP_ */
