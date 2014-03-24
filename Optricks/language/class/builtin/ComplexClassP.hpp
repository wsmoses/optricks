/*
 * ComplexClassP.hpp
 *
 *  Created on: Jan 29, 2014
 *      Author: Billy
 */

#ifndef COMPLEXCLASSP_HPP_
#define COMPLEXCLASSP_HPP_
#include "./ComplexClass.hpp"
#include "../../data/literal/ImaginaryLiteral.hpp"
#include "../../data/literal/IntLiteral.hpp"
#include "../../data/ConstantData.hpp"
#include "../../data/LocationData.hpp"

const Data* ComplexClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const{
	if(s!="real" && s!="imag"){
		illegalLocal(id,s);
		exit(1);
	}
	assert(instance->type==R_IMAG || instance->type==R_LOC || instance->type==R_CONST);
	assert(instance->getReturnType()==this);
	if(instance->type==R_IMAG){
		ImaginaryLiteral* cl = (ImaginaryLiteral*)instance;
		if(s=="real") return new ConstantData(innerClass->getZero(id), innerClass);
		else{
			return cl->imag->castTo(r, innerClass, id);
		}
	} else if(instance->type==R_CONST){
		Value* v = ((ConstantData*)instance)->value;
		return new ConstantData(r.builder.CreateExtractElement(v,getInt32((s=="real")?0:1)),this);

	} else {
		assert(instance->type==R_LOC);
		auto LD = ((const LocationData*)instance)->value;
		return new LocationData(LD->getInner(r, id, 0, (s=="real")?0:1), innerClass);
	}
	exit(1);
}


#endif /* COMPLEXCLASSP_HPP_ */
