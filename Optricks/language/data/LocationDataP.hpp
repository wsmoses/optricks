/*
 * LocationDataP.hpp
 *
 *  Created on: Jan 22, 2014
 *      Author: Billy
 */

#ifndef LOCATIONDATAP_HPP_
#define LOCATIONDATAP_HPP_
#include "./LocationData.hpp"
inline Value* LocationData::castToV(RData& r, const AbstractClass* const right, const PositionID id) const {
		assert(value);
		if(type == right)
			return value->getValue(r,id);
		if((type->layout==POINTER_LAYOUT && right->layout==POINTER_LAYOUT) || (type->layout==PRIMITIVEPOINTER_LAYOUT && right->layout==PRIMITIVEPOINTER_LAYOUT)){
			if(type->hasSuper(right)) return r.builder.CreatePointerCast(value->getValue(r, id), right->type);
			else id.error("Cannot cast value of type "+type->getName()+" to "+right->getName());
		}
		return type->castTo(right, r, id, value->getValue(r,id));
	}


	inline const ConstantData* LocationData::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
		/*if(right->noopCast())
		if((type->layout==POINTER_LAYOUT && right->layout==POINTER_LAYOUT) || (type->layout==PRIMITIVEPOINTER_LAYOUT && right->layout==PRIMITIVEPOINTER_LAYOUT)){
			if(type->hasSuper(right)) return new ConstantData(value, right);
			else id.error("Cannot cast value of type "+type->name+" to "+right->name);
		}*/
		return new ConstantData(type->castTo(right, r, id, value->getValue(r,id)), right);
	}


#endif /* LOCATIONDATAP_HPP_ */
