/*
 * RData.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef RDATA_HPP_
#define RDATA_HPP_

#include "settings.hpp"

DATA DATA::toLocation(RData& m){
	if(type==R_LOC) return *this;
	if(type!=R_CONST){
		cerr << "Cannot make non-value / non-loc to a location" << type << endl << flush;
		exit(1);
	}
	assert(data.pointer!=NULL && "Cannot get location of NULL");
	AllocaInst* loc = m.builder.CreateAlloca(data.constant->getType(),0);
	assert(loc!=NULL);
	m.builder.CreateStore(data.constant, loc);
	return getLocation(loc, getReturnType());
}
Value* DATA::getValue(RData& r) const{
	assert(data.pointer!=NULL);
	if(type==R_CONST) return data.constant;
	else if(type==R_FUNC) return data.function;
	else if(type==R_LOC) return r.builder.CreateLoad(data.location);
	else{
		cerr << "Cannot get Value of DataType " << type << endl << flush;
		exit(1);
	}
};
void DATA::setValue(RData& r, Value* v){
	assert(data.pointer!=NULL && "Cannot set NULL DATA");
	if(type==R_LOC) r.builder.CreateStore(v, data.location);
	else{
		cerr << "Cannot set Value of DataType " << type << endl << flush;
		exit(1);
	}
};

#include "ClassProto.hpp"
BasicBlock* RData::getBlock(String name, JumpType jump, BasicBlock* bb, DATA val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume){
	if(name==""){
		if(jump==RETURN || jump==YIELD){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == FUNC || jumps[i]->toJump==GENERATOR){
					if(jumps[i]->returnType==voidClass){
						if(val.getType()!=R_UNDEF && val.getReturnType()!=voidClass) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<BasicBlock*,DATA>(bb, DATA::getNull()));
					}
					else jumps[i]->endings.push_back(std::pair<BasicBlock*,DATA>(bb, val.castTo(*this, jumps[i]->returnType, id).toValue(*this)));
					jumps[i]->resumes.push_back(resume);
					return jumps[i]->end;
				}
				if(i <= 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i <= 0){
					cerr << "Error could not find continue/break block" << endl << flush;
					return NULL;
				}
			}
		}
	} else {
		cerr << "Error not done yet2" << endl << flush;
		exit(1);
		return NULL;
	}
}

#endif /* RDATA_HPP_ */
