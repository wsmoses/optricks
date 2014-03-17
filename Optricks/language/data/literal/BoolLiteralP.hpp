/*
 * BoolLiteral.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: Billy
 */
#ifndef BOOLLITERALP_HPP_
#define BOOLLITERALP_HPP_
#include "BoolLiteral.hpp"
#include "../../class/builtin/BoolClass.hpp"
const AbstractClass* BoolLiteral::getReturnType() const{
	return &boolClass;
}
ConstantInt* BoolLiteral::getValue(RData& r, PositionID id) const{
	return ConstantInt::get(BOOLTYPE,value);
}
const Literal* BoolLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	if(right->classType!=CLASS_BOOL) id.error("Cannot cast bool to "+right->getName());
	return this;
}
ConstantInt* BoolLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	if(right->classType!=CLASS_BOOL) id.error("Cannot cast bool to "+right->getName());
	return ConstantInt::get(BOOLTYPE,value);
}
#endif
