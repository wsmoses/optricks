/*
 * VectorClass.hpp
 *
 *  Created on: Mar 6, 2014
 *      Author: Billy
 */

#ifndef VECTORCLASS_HPP_
#define VECTORCLASS_HPP_

#include "../AbstractClass.hpp"
#include "./IntClass.hpp"

//todo force vector of int / rational / complex
class VectorClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const d, uint64_t l){
		assert(d);
		assert(l>0);
		std::stringstream ss;
		ss << "vector{" << d->getName() << "," << l << "}";
		return ss.str();
	}
	static inline Type* getVectorType(const AbstractClass* const d, uint64_t l){
		if(d->classType==CLASS_INT || d->classType==CLASS_FLOAT){
			return VectorType::get(d->type, l);
		} else{
			PositionID(0,0,"#vector").compilerError("Vectors not implemented");
			exit(1);
		}
	}
	const AbstractClass* inner;
	const uint64_t len;
protected:
	VectorClass(const AbstractClass* a,uint64_t le):
		AbstractClass(nullptr,str(a,le),nullptr,PRIMITIVE_LAYOUT,CLASS_ARRAY,true,getVectorType(a,le)),inner(a),len(le){
		assert(a->classType!=CLASS_LAZY);
		assert(a->classType!=CLASS_REF);
		assert(le>0);
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_VECTOR: {
			//if(!inner) return true;
			const VectorClass* tc = (const VectorClass*)toCast;
			if(!inner->hasCast(tc->inner)) return false;
			return tc->len==len;
		}
		default:
			return false;
		}
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		else return IntLiteralClass::get(len);
	}
	bool hasLocalData(String s) const override final{
		return s=="length";
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		return new IntLiteral(len);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_VECTOR: {
			const VectorClass* tc = (const VectorClass*)toCast;
			if(!inner->hasCast(tc->inner)) return false;
			return tc->len==len;
		}
		default:
			return false;
		}
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const;

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_VECTOR);
		assert(b->classType==CLASS_VECTOR);
		assert(hasCast(a));
		assert(hasCast(b));
		const VectorClass* fa = (const VectorClass*)a;
		const VectorClass* fb = (const VectorClass*)b;
		return inner->compare(fa->inner, fb->inner);
	}
	static VectorClass* get(const AbstractClass* args, uint64_t l) {
		static std::map<const AbstractClass*,std::map<uint64_t, VectorClass*>> mp;
		auto tmp = mp.find(args);
		if(tmp==mp.end()){
			return mp[args][l] = new VectorClass(args,l);
		}
		auto tmp2 = tmp->second;
		auto fd = tmp2.find(l);
		if(fd==tmp2.end()){
			return tmp2[l] = new VectorClass(args,l);
		} else return fd->second;
	}
};



#endif /* VECTORCLASS_HPP_ */
