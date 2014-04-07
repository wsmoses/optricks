/*
 * FloatLiteral.cpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALP_HPP_
#define FLOATLITERALP_HPP_
#include "FloatLiteral.hpp"
#include "ImaginaryLiteral.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/literal/FloatLiteralClass.hpp"

//TODO separate "constant" floats from float literals
const AbstractClass* FloatLiteral::getReturnType() const{
	return &floatLiteralClass;
}
ConstantFP* FloatLiteral::getValue(RData& r, PositionID id) const{
	id.error("Cannot getValue of class without type");
	exit(1);
}
const Data* FloatLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	if(right->classType==CLASS_FLOATLITERAL) return this;
	switch(right->classType){
	case CLASS_VOID: return &VOID_DATA;
		case CLASS_FLOAT:{
			return new ConstantData(((const FloatClass*)right)->getValue(id,value), right);
		}
		case CLASS_COMPLEX:{
			return new ConstantData(((const ComplexClass*)right)->getValue(id, value), right);
		}
		default:
			id.error("Cannot cast floating-point literal to non-float type "+right->getName());
			exit(1);
	}
}
Constant* FloatLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	switch(right->classType){
		case CLASS_FLOAT:{
			return ((const FloatClass*)right)->getValue(id,value);
		}
		case CLASS_COMPLEX:{
			return ((const ComplexClass*)right)->getValue(id, value);
		}
		default:
			id.error("Cannot cast floating-point literal to non-float type "+right->getName());
			exit(1);
	}
}

bool FloatLiteral::hasCastValue(const AbstractClass* const a) const{
	return floatLiteralClass.hasCast(a);
}

int FloatLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	return floatLiteralClass.compare(a,b);
}
#endif /* FLOATLITERAL_CPP_ */
