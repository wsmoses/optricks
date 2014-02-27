/*
 * FloatLiteral.cpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALP_HPP_
#define FLOATLITERALP_HPP_
#include "FloatLiteral.hpp"
#include "ComplexLiteral.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/literal/LiteralClass.hpp"
const AbstractClass* FloatLiteral::getReturnType() const{
	if(floatType) return floatType;
	else{
		return floatLiteralClass;
	}
}
Constant* FloatLiteral::getValue(RData& r, PositionID id) const{
	if(floatType==nullptr){
		id.error("Cannot getValue of class without type");
		exit(1);
	} else {
		switch(floatValueType){
		case FLOAT_PI:
			return floatType->getPi();
		case FLOAT_E:
			return floatType->getE();
		case FLOAT_CATALAN:
			return floatType->getCatalan();
		case FLOAT_LN2:
			return floatType->getLN2();
		case FLOAT_NORMAL:
		default:
			return floatType->getValue(value);
		}
	}
}
const Literal* FloatLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
		case CLASS_FLOAT:{
			if(right==floatClass) return this;
			if(floatType!=nullptr && !floatType->hasCast(right))
					id.error("Cannot cast float type "+floatType->getName()+" to "+right->getName());
			return new FloatLiteral(value, floatValueType, (FloatClass*)right);
		}
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			if(floatType!=nullptr && !floatType->hasCast(cc->innerClass))
					id.error("Cannot cast float type "+floatType->getName()+" to "+right->getName());
			return new ComplexLiteral(this,nullptr,cc);
		}
		default:
			id.error("Cannot cast floating-point literal to non-float type "+right->getName());
			exit(1);
		}
}
Constant* FloatLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	switch(right->classType){
	case CLASS_FLOAT:{
		FloatClass* fc = ((FloatClass*)right);
		if(floatType!=nullptr && !floatType->hasCast(fc))
					id.error("Cannot cast float type "+floatType->getName()+" to "+fc->getName());
		switch(floatValueType){
		case FLOAT_PI: return fc->getPi();
		case FLOAT_E: return fc->getE();
		case FLOAT_LN2: return fc->getLN2();
		case FLOAT_CATALAN: return fc->getCatalan();
		case FLOAT_NORMAL: return fc->getValue(value);break;
		}
		exit(1);
	}
	case CLASS_COMPLEX:{
		ComplexClass* cc = (ComplexClass*)right;
		if(cc->innerClass->classType!=CLASS_FLOAT) id.error("Cannot promote floating-point literal to complex type "+cc->getName());
		if(floatType!=nullptr && !floatType->hasCast(cc->innerClass))
			id.error("Cannot cast float type "+floatType->getName()+" to "+cc->getName());
		FloatClass* fc = ((FloatClass*)cc->innerClass);
		Constant* re;
		switch(floatValueType){
		case FLOAT_PI: re= fc->getPi();break;
		case FLOAT_E: re=fc->getE();break;
		case FLOAT_LN2: re=fc->getLN2();break;
		case FLOAT_CATALAN: re=fc->getCatalan();break;
		case FLOAT_NORMAL: re=fc->getValue(value);break;
		}
		Constant* c[2] = { re, fc->getZero()};
		return ConstantVector::get(ArrayRef<Constant*>(c));
	}
	default:
		id.error("Cannot cast floating-point literal to non-float type "+right->getName());
		exit(1);
	}
}

bool FloatLiteral::hasCastValue(const AbstractClass* const a) const{
		return a->classType==CLASS_FLOAT || (a->classType==CLASS_COMPLEX && ((ComplexClass*)a)->innerClass->classType==CLASS_FLOAT) ;
	}

int FloatLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(a->classType==CLASS_FLOAT || a->classType==CLASS_COMPLEX);
	assert(b->classType==CLASS_FLOAT || b->classType==CLASS_COMPLEX);
	if(a->classType==b->classType){
		if(a->classType==CLASS_COMPLEX){
			ComplexClass* ca = (ComplexClass*)a;
			ComplexClass* cb = (ComplexClass*)b;
			assert(ca->innerClass->classType==CLASS_FLOAT);
			assert(cb->innerClass->classType==CLASS_FLOAT);
			return 0;
		} else{
			return 0;
		}
	} else return (a->classType==CLASS_FLOAT)?(-1):(1);
}
#endif /* FLOATLITERAL_CPP_ */
