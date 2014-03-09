/*
 * NamedTupleClass.hpp
 *
 *  Created on: Jan 21, 2014
 *      Author: Billy
 */

#ifndef NAMEDTUPLECLASS_HPP_
#define NAMEDTUPLECLASS_HPP_
#include "./TupleClass.hpp"

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
	static inline Type* getTupleType(const std::vector<const AbstractClass*>& args,const std::vector<String>& b){
		const auto len = args.size();
		if(len==1) return args[0]->type;
		Type* ar[len];
		for(unsigned int i=0; i<len; i++){
			assert(args[i]->classType!=CLASS_LAZY);
			ar[i]=args[i]->type;
		}
		return StructType::create(ArrayRef<Type*>(ar, len),StringRef(str(args,b)),false);
	}
	const std::vector<String> innerNames;
private:
	NamedTupleClass(const std::vector<const AbstractClass*>& args,const std::vector<String>& b):
		TupleClass(args,CLASS_NAMED_TUPLE),innerNames(b){
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
				assert(instance->type==R_LOC || instance->type==R_CONST || instance->type==R_TUPLE);
				if(instance->type==R_TUPLE){
					TupleData* td = (TupleData*)instance;
					return td->inner[i]->castTo(r,innerTypes[i],id);
				}
				assert(instance->getReturnType()==this);
				if(instance->type==R_LOC){
					//TODO location
					id.compilerError("NTuple TODO:// allow getting named tuple class data from location");
					exit(1);
				} else{
					assert(instance->type==R_CONST);
					Value* v = ((ConstantData*)instance)->value;
					return new ConstantData(r.builder.CreateExtractValue(v,i),innerTypes[i]);
				}
			}
		illegalLocal(id,s);
		exit(1);
	}
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		for(unsigned int i=0; i<innerNames.size(); i++)
			if(innerNames[i]==s)
				return std::pair<AbstractClass*,unsigned int>(innerTypes[i],i);
		illegalLocal(id,s);
		exit(1);
	}*/
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType!=CLASS_NAMED_TUPLE) return false;
		NamedTupleClass* tc = (NamedTupleClass*)toCast;
		if(tc->innerTypes.size()!=innerTypes.size()) return false;
		for(unsigned i=0; i<innerTypes.size(); i++){
			if(!innerTypes[i]->noopCast(tc->innerTypes[i])) return false;
			if(innerNames[i]!=tc->innerNames[i]) return false;
		}
		return true;
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const;

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_NAMED_TUPLE);
		assert(b->classType==CLASS_NAMED_TUPLE);
		assert(hasCast(a));
		assert(hasCast(b));
		if(a==this)
			return (b==this)?(0):(-1);
		else return (b==this)?(1):(0);
	}
	static NamedTupleClass* get(const std::vector<const AbstractClass*>& args,const std::vector<String>& b) {
		static Mapper<const AbstractClass*,Mapper<String, NamedTupleClass*>> map;
		NamedTupleClass*& fc = map.get(args).get(b);
		if(fc==nullptr) fc = new NamedTupleClass(args,b);
		return fc;
	}
};




#endif /* NAMEDTUPLECLASS_HPP_ */
