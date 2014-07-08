/*
 * HashMapClass.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: Billy
 */

#ifndef HASHMAPCLASS_HPP_
#define HASHMAPCLASS_HPP_

#include "../AbstractClass.hpp"
#include "./IntClass.hpp"
class HashMapClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const c, const AbstractClass* const d){
		assert(c);
		assert(d);
		return "map{"+c->getName()+","+d->getName()+"}";
	}
	static inline llvm::StructType* getNodeType(const AbstractClass* const c, const AbstractClass* const d){
		auto T = llvm::StructType::create(llvm::getGlobalContext(), llvm::StringRef("map.Node{"+c->getName()+","+d->getName()+"}"));
		llvm::SmallVector<llvm::Type*,3> types(3);
		types[0] = llvm::PointerType::getUnqual(T);
		types[1] = c->type;
		types[2] = d->type;
		T->setBody(types,false);
		return T;
	}
	static inline llvm::Type* getMapType(const AbstractClass* const c, const AbstractClass* const d){
		llvm::SmallVector<llvm::Type*,4> ar(4);
		ar[0] = /* Counts (for garbage collection) */ intClass.type;
		ar[1] = /* Length of array */ intClass.type;
		ar[2] = /* Amount of memory allocated */ intClass.type;
		ar[3] = /* Actual data */ llvm::PointerType::getUnqual(llvm::PointerType::getUnqual(getNodeType(c,d)));
		return llvm::PointerType::getUnqual(llvm::StructType::get(llvm::getGlobalContext(),ar,false));
	}
	llvm::StructType* const nodeType;
	const AbstractClass* key;
	const AbstractClass* value;
protected:
	HashMapClass(const AbstractClass* a,const AbstractClass* b);
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_HASHMAP: {
			//if(!inner) return true;
			const HashMapClass* tc = (const HashMapClass*)toCast;
			return key->hasCast(tc->key) && value->hasCast(tc->value);
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s!="length" && s!="alloced"){
			illegalLocal(id,s);
			exit(1);
		}
		return &intClass;
	}
	bool hasLocalData(String s) const override final{
		return s=="length" || s=="alloced";
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		//TODO reference count carr / make into int[len]&
		if(s=="alloced"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2)), &intClass);
		}
		if(s!="length"){
			illegalLocal(id,s);
			exit(1);
		}
		llvm::Value* V = instance->getValue(r,id);
		return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1)), &intClass);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_ARRAY: {
			//if(!inner) return true;
			HashMapClass* tc = (HashMapClass*)toCast;
			return key->noopCast(tc->key) && value->noopCast(tc->value);
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}

	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		if(toCast==this) return valueToCast;
		if(toCast->classType!=CLASS_HASHMAP){
			id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		auto AR = (const HashMapClass*)toCast;
		if(!key->hasCast(AR->key) || !value->hasCast(AR->value)){
			id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		if(key->noopCast(AR->key) && value->noopCast(AR->value)){
			assert(toCast->type->isPointerTy());
			return r.pointerCast(valueToCast, (llvm::PointerType*) toCast->type);
		}
		cerr << this << " " << toCast << endl << flush;
		id.compilerError("Casting hashmap types has not been implemented "+toCast->getName());
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		HashMapClass* fa = (HashMapClass*)a;
		HashMapClass* fb = (HashMapClass*)b;
		auto NEX = key->compare(fa->key, fb->key);
		if(NEX==0){
			return value->compare(fa->value,fb->value);
		} else return NEX;
	}
	static HashMapClass* get(const AbstractClass* a, const AbstractClass* b) {
		assert(a);
		assert(b);
		static std::map<const AbstractClass*,std::map<const AbstractClass*, HashMapClass*>> mp;
		auto tmp = mp.find(a);
		if(tmp==mp.end()){
			return mp[a][b] = new HashMapClass(a,b);
		}
		auto tmp2 = tmp->second;
		auto fd = tmp2.find(b);
		if(fd==tmp2.end()){
			return tmp2[b] = new HashMapClass(a,b);
		} else return fd->second;
	}
};

#endif /* HASHMAPCLASS_HPP_ */
