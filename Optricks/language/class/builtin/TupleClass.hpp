/*
 * TupleClass.hpp
 *
 *  Created on: Jan 20, 2014
 *      Author: Billy
 */

#ifndef TUPLECLASS_HPP_
#define TUPLECLASS_HPP_
#include "../AbstractClass.hpp"

class TupleClass: public AbstractClass{
public:
	static inline String str(const std::vector<const AbstractClass*>& a){
		String s= "(";
		bool first=true;
		for(const auto& b: a){
			if(first) first=false;
			else s+=",";
			s+=b->getName();
		}
		return s+")";
	}
	static inline Type* getTupleType(const std::vector<const AbstractClass*>& args){
		const auto len = args.size();
		if(len==1) return args[0]->type;
		Type* ar[len];
		for(unsigned int i=0; i<len; i++){
			assert(args[i]->classType!=CLASS_LAZY);
			ar[i]=args[i]->type;
		}
		return StructType::create(ArrayRef<Type*>(ar, len),StringRef(str(args)),false);
	}
	const std::vector<const AbstractClass*> innerTypes;
protected:
	TupleClass(const std::vector<const AbstractClass*>& args,ClassType ct=CLASS_TUPLE):
		AbstractClass(nullptr,str(args),nullptr,PRIMITIVE_LAYOUT,ct,true,getTupleType(args)),innerTypes(args){
		assert(ct==CLASS_TUPLE || ct==CLASS_NAMED_TUPLE);
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_NAMED_TUPLE:
		case CLASS_TUPLE: {
			TupleClass* tc = (TupleClass*)toCast;
			if(tc->innerTypes.size()!=innerTypes.size()) return false;
			for(unsigned i=0; i<innerTypes.size(); i++){
				if(!innerTypes[i]->hasCast(tc->innerTypes[i])) return false;
			}
			return true;
		}
		default:
			return false;
		}
	}

	bool hasLocalData(String s) const override {
		if(s.length()<2 || s[0]!='_') return false;
		unsigned int i=0;
		unsigned int p=1;
		do{
			if(s[p]<'0' || s[p]>'9') return false;
			i*=10;
			i+= (s[p]-'0');
		}while(p<s.length());
		if(i>=innerTypes.size()) return false;
		return true;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
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
		}while(p<s.length());
		if(i>=innerTypes.size()){
			illegalLocal(id,s);
			exit(1);
		}
		return innerTypes[i];
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override;
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override{
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
		}while(p<s.length());
		if(i>=innerTypes.size()){
			illegalLocal(id,s);
			exit(1);
		}
		return std::pair<AbstractClass*,unsigned int>(innerTypes[i],i);
	}*/
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType!=CLASS_TUPLE && toCast->classType!=CLASS_NAMED_TUPLE) return false;
		TupleClass* tc = (TupleClass*)toCast;
		if(tc->innerTypes.size()!=innerTypes.size()) return false;
		for(unsigned i=0; i<innerTypes.size(); i++){
			if(!innerTypes[i]->noopCast(tc->innerTypes[i])) return false;
		}
		return true;
	}
	inline Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const;

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_TUPLE || a->classType==CLASS_NAMED_TUPLE);
		assert(b->classType==CLASS_TUPLE || b->classType==CLASS_NAMED_TUPLE);
		assert(hasCast(a));
		assert(hasCast(b));
		TupleClass* fa = (TupleClass*)a;
		TupleClass* fb = (TupleClass*)b;
		assert(fa->innerTypes.size() == innerTypes.size());
		assert(fb->innerTypes.size() == innerTypes.size());
		bool aBetter = false;
		bool bBetter = false;
		if(a->classType==CLASS_NAMED_TUPLE || b->classType==CLASS_NAMED_TUPLE) return 0;
		for(unsigned i=0; i<innerTypes.size(); i++){
			auto j = innerTypes[i]->compare(fa->innerTypes[i], fb->innerTypes[i]);
			if(j!=0){
				if(j<0){
					if(bBetter) return 0;
					aBetter = true;
				} else {
					if(aBetter) return 0;
					bBetter = true;
				}
			}
		}
		if(aBetter) return -1;
		else if(bBetter) return 1;
		else return 0;
	}
	static TupleClass* get(const std::vector<const AbstractClass*>& args) {
		static Mapper<const AbstractClass*, TupleClass*> map;
		TupleClass*& fc = map.get(args);
		if(fc==nullptr) fc = new TupleClass(args);
		return fc;
	}
};




#endif /* TUPLECLASS_HPP_ */
