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

const AbstractClass* ComplexLiteral::getReturnType() const{
	if(complexType) return complexType;
	else{
		const AbstractClass* rc = real->getReturnType();
		assert(rc);
		const AbstractClass* ic = real->getReturnType();
		assert(ic);
		return complexLiteralClass;
	}
}

Value* ComplexLiteral::getValue(RData& r, PositionID id) const{
	if(complexType==NULL) {
		id.error("Cannot getValue of complex class without type");
		exit(1);
	} else {
		Value* a = real->castToV(r, complexType->innerClass, id);
		Value* b = imag->castToV(r, complexType->innerClass, id);
		/*if(Constant* a1 = dynamic_cast<Constant>(a)){
			if(Constant* b1 = dynamic_cast<Constant>(b)){
				return ConstantVector::get(ArrayRef<Constant*>({a,b}));
			}
		}*/
		Value* vec = UndefValue::get(complexType->type);
		return r.builder.CreateInsertElement(r.builder.CreateInsertElement(
				UndefValue::get(complexType->type), a, getInt32(0)), b, getInt32(1));
	}
}

const ComplexLiteral* ComplexLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
	case CLASS_COMPLEX:{
		ComplexClass* cc = (ComplexClass*)right;
		if(complexType!=nullptr && !complexType->innerClass->hasCast(cc->innerClass))
			id.error("Cannot cast complex type "+complexType->getName()+" to "+cc->getName());
		auto nreal = real->castTo(r, cc->innerClass, id);
		auto nimag = imag->castTo(r, cc->innerClass, id);
		if(nreal==real && nimag == imag) return this;
		else return new ComplexLiteral(nreal, nimag, cc);
	}
	default:
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
}

Value* ComplexLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	switch(right->classType){
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			if(complexType!=nullptr && !complexType->innerClass->hasCast(cc->innerClass))
				id.error("Cannot cast complex type "+complexType->getName()+" to "+cc->getName());

			Value* a = real->castToV(r, cc->innerClass, id);
			Value* b = imag->castToV(r, cc->innerClass, id);
			/*if(Constant* a1 = dynamic_cast<Constant>(a)){
				if(Constant* b1 = dynamic_cast<Constant>(b)){
					return ConstantVector::get(ArrayRef<Constant*>({a,b}));
				}
			}*/
			Value* vec = UndefValue::get(complexType->type);
			return r.builder.CreateInsertElement(r.builder.CreateInsertElement(
					UndefValue::get(complexType->type), a, getInt32(0)), b, getInt32(1));
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
