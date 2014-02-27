/*
 * ComplexLiteralP.hpp
 *
 *  Created on: Jan 4, 2014
 *      Author: Billy
 */

#ifndef COMPLEXLITERALP_HPP_
#define COMPLEXLITERALP_HPP_
#include "ComplexLiteral.hpp"
#include "../../class/builtin/IntClass.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/builtin/ComplexClass.hpp"
#include "../../class/builtin/RationalClass.hpp"
#include "../../class/literal/LiteralClass.hpp"

const AbstractClass* ComplexLiteral::getReturnType() const{
	if(complexType) return complexType;
	else{
		return complexLiteralClass;
	}
}

Constant* ComplexLiteral::getValue(RData& r, PositionID id) const{
	if(complexType==NULL) {
		id.error("Cannot getValue of complex class without type");
		exit(1);
	} else {
		Constant* z[2] = { real->castToV(r, complexType->innerClass, id),imag->castToV(r, complexType->innerClass, id)};
		return ConstantVector::get(ArrayRef<Constant*>(z));
	}
}

const ComplexLiteral* ComplexLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
	case CLASS_COMPLEX:{
		ComplexClass* cc = (ComplexClass*)right;
		if(complexType!=nullptr && !complexType->innerClass->hasCast(cc->innerClass))
			id.error("Cannot cast complex type "+complexType->getName()+" to "+cc->getName());
		if(real->type==R_INT){
			IntClass* ic = ((IntLiteral*)real)->intType;
			if(ic && !ic->hasCast(cc->innerClass))
				id.error("Cannot promote real part of complex literal of type "+ic->getName()+" to "+cc->innerClass->getName());
		} else{
			assert(real->type==R_FLOAT);
			FloatClass* ic = ((FloatLiteral*)real)->floatType;
			if(ic && !ic->hasCast(cc->innerClass))
				id.error("Cannot promote real part of complex literal of type "+ic->getName()+" to "+cc->innerClass->getName());
		}
		if(real->type==R_INT){
			IntClass* ic = ((IntLiteral*)imag)->intType;
			if(ic && !ic->hasCast(cc->innerClass))
				id.error("Cannot promote imaginary part of complex literal of type "+ic->getName()+" to "+cc->innerClass->getName());
		} else{
			assert(real->type==R_FLOAT);
			FloatClass* ic = ((FloatLiteral*)imag)->floatType;
			if(ic && !ic->hasCast(cc->innerClass))
				id.error("Cannot promote imaginary part of complex literal of type "+ic->getName()+" to "+cc->innerClass->getName());
		}
		return new ComplexLiteral(real, imag,cc);
	}
	default:
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
}

Constant* ComplexLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	switch(right->classType){
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			if(complexType!=nullptr && !complexType->innerClass->hasCast(cc->innerClass))
				id.error("Cannot cast complex type "+complexType->getName()+" to "+cc->getName());
			Constant* z[2] = { real->castToV(r, cc->innerClass, id),imag->castToV(r, cc->innerClass, id)};
			return ConstantVector::get(ArrayRef<Constant*>(z));
		}
		default:
			id.error("Cannot promote complex type to non-complex type "+right->getName());
			exit(1);
		}
}

bool ComplexLiteral::hasCastValue(const AbstractClass* const a) const{
	if(a->classType!=CLASS_COMPLEX) return false;
	ComplexClass* cc = (ComplexClass*)a;
	return real->hasCastValue(cc->innerClass) && imag->hasCastValue(cc->innerClass);
}

int ComplexLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(a->classType==CLASS_COMPLEX);
	assert(b->classType==CLASS_COMPLEX);
	ComplexClass* ca = (ComplexClass*)a;
	ComplexClass* cb = (ComplexClass*)a;
	auto r = real->compareValue(ca->innerClass, cb->innerClass);
	auto i = imag->compareValue(ca->innerClass, cb->innerClass);
	if((r>0 && i<0) || (r<0 && i>0)) return 0;
	else if(r<0 && i<0) return -1;
	else if(r>0 && i>0) return 1;
	else if(r==0) return i;
	else{
		assert(i==0);
		return r;
	}
}
#endif /* COMPLEXLITERALP_HPP_ */
