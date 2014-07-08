/*
 * ArrayClass.hpp
 *
 *  Created on: Feb 7, 2014
 *      Author: Billy
 */

#ifndef ARRAYCLASS_HPP_
#define ARRAYCLASS_HPP_
#include "../AbstractClass.hpp"
#include "./IntClass.hpp"
class ArrayClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const d){
		if(!d) return "array{#}";
		assert(d);
		//if(d==nullptr) return "array";
		/** length 0 is arbitrary-length **/
		return d->getName()+"[]";
	}
	static inline llvm::Type* getArrayType(const AbstractClass* const d){
		if(!d) { return VOIDTYPE;
		//} else if(l>0){
		//	llvm::ArrayType* at = llvm::ArrayType::get((llvm::Type*) ( d->type) ,l);
		//	return at;
		} else {
			llvm::SmallVector<llvm::Type*,4> ar(4);
			ar[0] = /* Counts (for garbage collection) */ intClass.type;
			ar[1] = /* Length of array */ intClass.type;
			ar[2] = /* Amount of memory allocated */ intClass.type;
			ar[3] = /* Actual data */ llvm::PointerType::getUnqual(d->type);
			return llvm::PointerType::getUnqual(llvm::StructType::get(llvm::getGlobalContext(), ar,false));
		}
	}
	const AbstractClass* inner;
	//const uint64_t len;
protected:
	ArrayClass(const AbstractClass* a);
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			if(!inner) return true;
			const ArrayClass* tc = (const ArrayClass*)toCast;
			if(!tc->inner) return false;
			return inner->hasCast(tc->inner);
			//if(!inner->hasCast(tc->inner)) return false;
			//if(tc->len==0) return true;
			//return tc->len==len;
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s=="carr") return &c_pointerClass;
		if(s=="alloced" && inner!=nullptr) return &intClass;
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		return &intClass;
	}
	bool hasLocalData(String s) const override final{
		return s=="length" || s=="carr" || (s=="alloced" && inner!=nullptr);
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		//TODO reference count carr / make into int[len]&
		if(s=="carr"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(
					r.pointerCast(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3)),C_POINTERTYPE),
					&c_pointerClass);
		}
		if(inner!=nullptr && s=="alloced"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2)), &intClass);
		}
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		if(inner!=nullptr){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1)), &intClass);
		} else return new ConstantData(getInt32(0), &intClass);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			if(!inner) return true;
			ArrayClass* tc = (ArrayClass*)toCast;
			if(!tc->inner) return false;
			return inner->noopCast(tc->inner);
			//if(!inner->noopCast(tc->inner)) return false;
			//if(tc->len==0) return true;
			//return tc->len==len;
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const;

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		ArrayClass* fa = (ArrayClass*)a;
		ArrayClass* fb = (ArrayClass*)b;
		if(!inner){
			if(!fa->inner && !fb->inner)
				return 0;
			else if(!fa->inner) return -1;
			else if(!fb->inner) return 1;
			return 0;
		}
		auto NEX = inner->compare(fa->inner, fb->inner);
		//if(NEX==0){
		//	if(fa->len==len){
		//		if(fb->len==len) return 0;
		//		else return -1;
		//	} else if(fb->len==len){
		//		return 1;
		//	} else return 0;
		//} else
			return NEX;
	}
	static ArrayClass* get(const AbstractClass* args) {
		if(args==nullptr){
			static ArrayClass* ac = new ArrayClass(nullptr);
			return ac;
		} else {
			static std::map<const AbstractClass*,ArrayClass*> mp;
			auto tmp = mp.find(args);
			if(tmp==mp.end()){
				return mp[args] = new ArrayClass(args);
			} else return tmp->second;
		}
	}
};




#endif /* ARRAYCLASS_HPP_ */
