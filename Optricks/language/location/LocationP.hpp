/*
 * LocationP.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef LOCATIONP_HPP_
#define LOCATIONP_HPP_
#include "./Location.hpp"

llvm::Value* StandardLocation::getValue(RData& r, PositionID id){
	assert(position);
	assert(position->getType()->isPointerTy());
	auto V = r.builder.CreateLoad(position);
	assert(V);
	assert(V->getType());
	return V;
}
void StandardLocation:: setValue(llvm::Value* v, RData& r){
	assert(position);
	assert(position->getType()->isPointerTy());
	r.builder.CreateStore(v, position);
}

/*
LazyLocation::LazyLocation(void*,RData& r,Value* p, BasicBlock* b,Value* d,bool u)
	:position(p){
	assert(position);
	assert(position->getType()->isPointerTy());
	if(b!=nullptr && d!= nullptr){
		if(r.builder.GetInsertBlock()!=b) PositionID(0,0,"#lazy").compilerError("Cannot fake lazy");
		setValue(d, r);
	}
}

Value* LazyLocation::getValue(RData& r, PositionID id){
	assert(position);
	assert(position->getType()->isPointerTy());
	auto V = r.builder.CreateLoad(position);
	assert(V);
	assert(V->getType());
	return V;
}
void LazyLocation:: setValue(Value* v, RData& r){
	assert(position);
	assert(position->getType()->isPointerTy());
	r.builder.CreateStore(v, position);
}*/

#endif /* LOCATIONP_HPP_ */
