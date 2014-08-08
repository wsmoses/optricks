/*
 * LocationP.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef LOCATIONP_HPP_
#define LOCATIONP_HPP_
#include "./Location.hpp"


		Location* UpgradeLocation::getInner(RData& r, PositionID id, unsigned idx){
			return new StandardLocation(true,r.builder.CreateConstGEP1_32(getPointer(r,id), idx));
		}
		Location* UpgradeLocation::getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2){
			return new StandardLocation(true,r.builder.CreateConstGEP2_32(getPointer(r,id), idx1, idx2));
		}
llvm::Value* UpgradeLocation::getValue(RData& r, PositionID id){
	auto position = getPointer(r, id);
	assert(position);
	if(!position->getType()->isPointerTy()){
		position->dump();
		cerr << endl << flush;
		position->getType()->dump();
		cerr << endl << flush;
	}
	assert(position->getType()->isPointerTy());
	auto V = r.builder.CreateLoad(position);
	assert(V);
	assert(V->getType());
	return V;
}
llvm::Value* StandardLocation::getValue(RData& r, PositionID id){
	assert(position);
	if(!position->getType()->isPointerTy()){
		position->dump();
		cerr << endl << flush;
		position->getType()->dump();
		cerr << endl << flush;
	}
	assert(position->getType()->isPointerTy());
	auto V = r.builder.CreateLoad(position);
	assert(V);
	assert(V->getType());
	return V;
}
void UpgradeLocation:: setValue(llvm::Value* v, RData& r){
	auto position = getPointer(r, PositionID(0,0,"#"));
	assert(position);
	assert(position->getType()->isPointerTy());
	r.builder.CreateStore(v, position);
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
