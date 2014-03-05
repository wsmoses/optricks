/*
 * LocationP.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef LOCATIONP_HPP_
#define LOCATIONP_HPP_
#include "./Location.hpp"

Value* StandardLocation::getValue(RData& r, PositionID id){
	return r.builder.CreateLoad(position);
}
void StandardLocation:: setValue(Value* v, RData& r){
	r.builder.CreateStore(v, position);
}

LazyLocation::LazyLocation(RData& r,Value* p, BasicBlock* b,Value* d,bool u)
	:position(p){
	if(b!=nullptr && d!= nullptr){
		if(r.builder.GetInsertBlock()!=b) PositionID(0,0,"#lazy").compilerError("Cannot fake lazy");
		setValue(d, r);
	}
}

Value* LazyLocation::getValue(RData& r, PositionID id){
	return r.builder.CreateLoad(position);
}
void LazyLocation:: setValue(Value* v, RData& r){
	r.builder.CreateStore(v, position);
}

#endif /* LOCATIONP_HPP_ */
