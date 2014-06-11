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
	static inline String str(const AbstractClass* const d, uint64_t l){
		assert(d);
		//if(d==nullptr) return "array";
		/** length 0 is arbitrary-length **/
		if(l==0) return d->getName()+"[]";
		else{
			std::stringstream ss;
			ss << d->getName() << '[' << l << ']';
			return ss.str();
		}
	}
	static inline llvm::Type* getArrayType(const AbstractClass* const d, uint64_t l){
		if(l>0){
			llvm::ArrayType* at = llvm::ArrayType::get((llvm::Type*) ( d->type) ,l);
			return at;
		} else {
			llvm::SmallVector<llvm::Type*,4> ar(4);
			ar[0] = /* Counts (for garbage collection) */ intClass.type;
			ar[1] = /* Length of array */ intClass.type;
			ar[2] = /* Amount of memory allocated */ intClass.type;
			ar[3] = /* Actual data */ llvm::PointerType::getUnqual(d->type);
			return llvm::PointerType::getUnqual(llvm::StructType::create(ar,llvm::StringRef(str(d,l)),false));
		}
	}
	const AbstractClass* inner;
	const uint64_t len;
protected:
	ArrayClass(const AbstractClass* a,uint64_t le):
		AbstractClass(nullptr,str(a,le),nullptr,PRIMITIVE_LAYOUT,CLASS_ARRAY,true,getArrayType(a,le)),inner(a),len(le){
		assert(a->classType!=CLASS_LAZY);
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			//if(!inner) return true;
			const ArrayClass* tc = (const ArrayClass*)toCast;
			if(!inner->hasCast(tc->inner)) return false;
			if(tc->len==0) return true;
			return tc->len==len;
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s=="carr") return &c_pointerClass;
		if(s=="alloced" && len==0 && inner!=nullptr) return &intClass;
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		if(len==0 && inner!=nullptr) return &intClass;
		else return & intLiteralClass;
	}
	bool hasLocalData(String s) const override final{
		return s=="length" || s=="carr" || (s=="alloced" && len==0 && inner!=nullptr);
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		//TODO reference count carr / make into int[len]&
		if(s=="carr"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(
					r.builder.CreatePointerCast(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3)),C_POINTERTYPE),
					&c_pointerClass);
		}
		if(len==0 && inner!=nullptr && s=="alloced"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2)), &intClass);
		}
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		if(len==0 && inner!=nullptr){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1)), &intClass);
		} else return new IntLiteral(len);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			//if(!inner) return true;
			ArrayClass* tc = (ArrayClass*)toCast;
			if(!inner->noopCast(tc->inner)) return false;
			if(tc->len==0) return true;
			return tc->len==len;
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
		auto NEX = inner->compare(fa->inner, fb->inner);
		if(NEX==0){
			if(fa->len==len){
				if(fb->len==len) return 0;
				else return -1;
			} else if(fb->len==len){
				return 1;
			} else return 0;
		} else return NEX;
	}
	static ArrayClass* get(const AbstractClass* args, uint64_t l) {
		if(args==nullptr){
			static ArrayClass* ac = new ArrayClass(nullptr,0);
			return ac;
		} else {
			static std::map<const AbstractClass*,std::map<uint64_t, ArrayClass*>> mp;
			auto tmp = mp.find(args);
			if(tmp==mp.end()){
				return mp[args][l] = new ArrayClass(args,l);
			}
			auto tmp2 = tmp->second;
			auto fd = tmp2.find(l);
			if(fd==tmp2.end()){
				return tmp2[l] = new ArrayClass(args,l);
			} else return fd->second;
		}
	}
};




#endif /* ARRAYCLASS_HPP_ */
