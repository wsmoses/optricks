/*
 * NamedTupleClass.hpp
 *
 *  Created on: Jan 21, 2014
 *      Author: Billy
 */

#ifndef NAMEDTUPLECLASS_HPP_
#define NAMEDTUPLECLASS_HPP_
#include "./TupleClass.hpp"
#include "../../data/LocationData.hpp"
#include "../../data/TupleData.hpp"
#include "../../data/DeclarationData.hpp"

class NamedTupleClass: public TupleClass{
public:
	static inline String str(const std::vector<const AbstractClass*>& a,const std::vector<String>& b){
		assert(a.size()==b.size());
		String s= "(";
		bool first=true;
		for(unsigned i=0; i<a.size(); i++){
			if(first) first=false;
			else s+=",";
			s+=a[i]->getName()+":"+b[i];
		}
		return s+")";
	}
	const std::vector<String> innerNames;
private:
	NamedTupleClass(const std::vector<const AbstractClass*>& args,const std::vector<String>& b):
		TupleClass(args,CLASS_NAMED_TUPLE,str(args,b)),innerNames(b){
		///register methods such as print / tostring / tofile / etc
		assert(args.size()==b.size());
		for(unsigned int i=0; i<b.size(); i++){
			assert(b[i].length()>0);
			for(unsigned j=0; j<i; j++)
				assert(b[i]!=b[j]);
		}
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_VOID: return true;
		case CLASS_NAMED_TUPLE:{
			NamedTupleClass* tc = (NamedTupleClass*)toCast;
			if(tc->innerTypes.size()!=innerTypes.size()) return false;
			for(unsigned i=0; i<innerTypes.size(); i++){
				if(!innerTypes[i]->hasCast(tc->innerTypes[i])) return false;
				if(innerNames[i]!=tc->innerNames[i]) return false;
			}
			return true;
		}
		default:
			return false;
		}
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		for(unsigned int i=0; i<innerNames.size(); i++)
			if(innerNames[i]==s)
				return innerTypes[i];
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		for(const auto& t: innerNames) if(t==s) return true;
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		for(unsigned int i=0; i<innerNames.size(); i++)
			if(innerNames[i]==s){
				assert(instance->type==R_DEC || instance->type==R_LOC || instance->type==R_CONST || instance->type==R_TUPLE);
				if(instance->type==R_TUPLE){
					auto td = (TupleData*)instance;
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
					auto v = ((ConstantData*)instance)->value;
					if(innerTypes.size()==1) return new ConstantData(v, innerTypes[0]);
					return new ConstantData(r.builder.CreateExtractValue(v,i),innerTypes[i]);
				}
			}
		illegalLocal(id,s);
		exit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		if(toCast->classType!=CLASS_NAMED_TUPLE) return false;
		const NamedTupleClass* tc = (const NamedTupleClass*)toCast;
		if(tc->innerTypes.size()!=innerTypes.size()) return false;
		for(unsigned i=0; i<innerTypes.size(); i++){
			if(!innerTypes[i]->noopCast(tc->innerTypes[i])) return false;
			if(innerNames[i]!=tc->innerNames[i]) return false;
		}
		return true;
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		if(toCast==this) return valueToCast;
		if(toCast->classType!=CLASS_NAMED_TUPLE) id.error("Cannot cast named tuple class to "+toCast->getName());
		const NamedTupleClass* tc = (const NamedTupleClass*)toCast;
		if(tc->innerTypes.size()!=innerTypes.size()) id.error(toStr("Cannot cast named tuple class of length ",innerTypes.size()," to named tuple class of length ",tc->innerTypes.size()));
		llvm::Value* V =llvm:: UndefValue::get(toCast->type);
		assert(valueToCast->getType()==type);
		for(unsigned i=0; i<innerTypes.size(); i++){
			if(innerNames[i]!=tc->innerNames[i]) id.error("Cannot cast named tuple "+getName()+" to "+tc->getName());
			llvm::Value* M = innerTypes[i]->castTo(tc->innerTypes[i], r, id, r.builder.CreateExtractElement(valueToCast, getInt32(i)));
			r.builder.CreateInsertElement(V, M, getInt32(i));
		}
		return V;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		if(a==this)
			return (b==this)?(0):(-1);
		else return (b==this)?(1):(0);
	}
	static NamedTupleClass* get(const std::vector<const AbstractClass*>& args,const std::vector<String>& b) {
		assert(args.size()==b.size());
		static Mapper<const AbstractClass*,Mapper<String, NamedTupleClass*>> map;
		NamedTupleClass*& fc = map.get(args).get(b);
		if(fc==nullptr) fc = new NamedTupleClass(args,b);
		return fc;
	}
};




#endif /* NAMEDTUPLECLASS_HPP_ */
