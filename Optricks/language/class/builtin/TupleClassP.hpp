/*
 * TupleClassP.hpp
 *
 *  Created on: Jan 21, 2014
 *      Author: Billy
 */

#ifndef TUPLECLASSP_HPP_
#define TUPLECLASSP_HPP_
#include "./TupleClass.hpp"
#include "../../data/TupleData.hpp"
#include "./NamedTupleClass.hpp"
#include "../../RData.hpp"
#include "../../data/LocationData.hpp"
#include "../../data/DeclarationData.hpp"

inline llvm::Value* TupleClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
	if(toCast==this) return valueToCast;
	switch(toCast->classType){
		case CLASS_TUPLE:
		case CLASS_NAMED_TUPLE:{
			TupleClass* tc = (TupleClass*)toCast;
			if(tc->innerTypes.size() != innerTypes.size()) illegalCast(id,toCast);
			unsigned i;
			if(classType==CLASS_NAMED_TUPLE){
				if(toCast->classType!=CLASS_NAMED_TUPLE) illegalCast(id,toCast);
				NamedTupleClass* ntc=(NamedTupleClass*)tc;
				NamedTupleClass* t = (NamedTupleClass*)this;
				for(unsigned int j=0; j<innerTypes.size(); j++){
					if(t->innerNames[j]!=ntc->innerNames[j])
						illegalCast(id,toCast);
				}
			}
			for(i=0; i<innerTypes.size(); i++){
				if(!innerTypes[i]->noopCast(tc->innerTypes[i])){
					break;
				}
			}
			if(i==innerTypes.size()){
				assert(type==toCast->type);
				return valueToCast;
			}
			else{
				if(innerTypes.size()==1){
					return innerTypes[0]->castTo(tc->innerTypes[0], r, id, valueToCast);
				}
				llvm::Value* res = llvm::UndefValue::get(tc->type);
				for(unsigned j=0; j<i; j++)
					r.builder.CreateInsertValue(res, r.builder.CreateExtractValue(valueToCast,j),j);
				for( ; i<innerTypes.size(); i++)
					r.builder.CreateInsertValue(res,
							innerTypes[i]->castTo(tc->innerTypes[i], r, id, r.builder.CreateExtractValue(valueToCast,i))
							,i);
				return res;
			}
		}
		default:
			id.error("Cannot cast value of type "+getName()+" to "+toCast->getName());
			exit(1);
		}
	}


const Data* TupleClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const {
	if(s.length()<2 || s[0]!='_'){
		illegalLocal(id,s);
		exit(1);
	}
	unsigned int i=0;
	unsigned int p=1;
	do{
		if(s[p]<'0' || s[p]>'9') illegalLocal(id,s);
		i*=10;
		i+= (s[p]-'0');
		p++;
	}while(p<s.length());
	if(i>=innerTypes.size()){
		illegalLocal(id,s);
		exit(1);
	}
	assert(instance->type==R_DEC || instance->type==R_LOC || instance->type==R_CONST || instance->type==R_TUPLE);
	if(instance->type==R_TUPLE){
		TupleData* td = (TupleData*)instance;
		return td->inner[i]->castTo(r,innerTypes[i],id);
	}
	assert(instance->getReturnType()==this);
	if(instance->type==R_LOC){
		auto LD = ((const LocationData*)instance)->value;
		if(innerTypes.size()==1) return new LocationData(LD, innerTypes[0]);
		else return new LocationData(LD->getInner(r, id, 0, i), innerTypes[i]);
	} else if(instance->type==R_DEC){
		auto LD = ((const DeclarationData*)instance)->value->fastEvaluate(r)->value;
		if(innerTypes.size()==1) return new LocationData(LD, innerTypes[0]);
		else return new LocationData(LD->getInner(r, id, 0, i), innerTypes[i]);
	} else {
		assert(instance->type==R_CONST);
		llvm::Value* v = ((ConstantData*)instance)->value;
		if(innerTypes.size()==1) return new ConstantData(v, innerTypes[0]);
		return new ConstantData(r.builder.CreateExtractValue(v,i),innerTypes[i]);
	}
}
#endif /* TUPLECLASSP_HPP_ */
