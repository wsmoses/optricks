/*
 * all.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef ALL_HPP_
#define ALL_HPP_
/*
#include "settings.hpp"
#include "ClassProto.hpp"
#include "Tuple.hpp"
#include "FunctionProto.hpp"
#include "GenericClass.hpp"
#include "operations.hpp"
#include "RData.hpp"
#include "types.hpp"
#include "operators.hpp"
#include "Location.hpp"
#include "Scopable.hpp"

Value* ClassProto::generateData(RData& r){
	Type* t = getType(r);
	assert(t!=NULL);
	if(layoutType==POINTER_LAYOUT){
		Value* da = r.builder.CreateCall(o_malloc,getUInt32(s));//TODO decide malloc/calloc
		Value* dat = r.builder.CreatePointerCast(da,t);
		r.builder.CreateStore(getUInt32(0), r.builder.CreateConstGEP2_32(dat,0,0));
		return dat;
	} else
	return UndefValue::get(t);
}

DATA ouopNullCast::apply(DATA a, RData& m, PositionID id){
	return DATA::getConstant(ConstantPointerNull::get((PointerType*)(to->getType(m))),to);
}
*/
#endif /* ALL_HPP_ */
