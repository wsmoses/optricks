/*
 * IntLiteral.cpp
 *
 *  Created on: Dec 22, 2013
 *      Author: Billy
 */
#ifndef IntLiteralP_HPP_
#define IntLiteralP_HPP_
#include "IntLiteral.hpp"
#include "FloatLiteral.hpp"
#include "ImaginaryLiteral.hpp"
#include "RationalLiteral.hpp"
#include "../../class/builtin/IntClass.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/builtin/ComplexClass.hpp"
#include "../../class/builtin/RationalClass.hpp"

	const Data* IntLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const {
		switch(right->classType){
		case CLASS_VOID: return &VOID_DATA;
		case CLASS_INTLITERAL: return this;
		case CLASS_FLOATLITERAL:{
			return new FloatLiteral(value);
		}
		case CLASS_BIGINT:{
			return new ConstantData(bigIntClass.getValue(r, value), &bigIntClass);
		}
		case CLASS_INT:{
			const IntClass* ic = (const IntClass*)right;
			return new ConstantData(ic->getValue(id,value), right);
		}
		case CLASS_FLOAT:{
			const FloatClass* fc = (const FloatClass*)right;
			return new ConstantData(fc->getValue(id, value), right);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			if(cc->innerClass->classType==CLASS_INTLITERAL){
				return new ImaginaryLiteral(this, &ZERO_LITERAL);
			} else if(cc->innerClass->classType==CLASS_FLOATLITERAL){
				return new ImaginaryLiteral(new FloatLiteral(value), &ZERO_LITERAL);
			}
			else return new ConstantData(cc->getValue(id, value), right);
		}
		default:
			id.error("Integer literal cannot be cast to "+right->getName());
			exit(1);
		}
	}

#endif
