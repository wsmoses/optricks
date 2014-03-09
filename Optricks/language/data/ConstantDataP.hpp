/*
 * ConstantDataP.hpp
 *
 *  Created on: Jan 17, 2014
 *      Author: Billy
 */

#ifndef CONSTANTDATAP_HPP_
#define CONSTANTDATAP_HPP_
#include "ConstantData.hpp"
#include "../class/AbstractClass.hpp"
	ConstantData::ConstantData(Value* const val, const AbstractClass* const cp):LLVMData(R_CONST, cp),value(val){
		assert(val); assert(cp); assert(cp->classType!=CLASS_CLASS); assert(cp->classType!=CLASS_FUNC);
		//assert(cp->classType!=CLASS_GEN);
		assert(cp->layout!=LITERAL_LAYOUT);
	}


	Value* ConstantData::castToV(RData& r, const AbstractClass* const right, const PositionID id) const {
		if(type == right) return value;
		if((type->layout==POINTER_LAYOUT && right->layout==POINTER_LAYOUT) || (type->layout==PRIMITIVEPOINTER_LAYOUT && right->layout==PRIMITIVEPOINTER_LAYOUT)){
			if(type->hasSuper(right)) return value;
			else id.error("Cannot cast value of type "+type->getName()+" to "+right->getName());
		}
		return type->castTo(right, r, id, value);
	}


	const Data* ConstantData::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
		if(type == right) return this;
		/*if(right->noopCast())
		if((type->layout==POINTER_LAYOUT && right->layout==POINTER_LAYOUT) || (type->layout==PRIMITIVEPOINTER_LAYOUT && right->layout==PRIMITIVEPOINTER_LAYOUT)){
			if(type->hasSuper(right)) return new ConstantData(value, right);
			else id.error("Cannot cast value of type "+type->name+" to "+right->name);
		}*/
		return new ConstantData(type->castTo(right, r, id, value), right);
	}

#endif /* CONSTANTDATAP_HPP_ */