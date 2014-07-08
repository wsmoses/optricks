/*
 * ReferenceClass.hpp
 *
 *  Created on: Feb 28, 2014
 *      Author: Billy
 */

#ifndef REFERENCECLASS_HPP_
#define REFERENCECLASS_HPP_
#include "../AbstractClass.hpp"

#define REFERENCECLASS_C_
class ReferenceClass: public AbstractClass{
public:
	const AbstractClass* const innerType;
	inline llvm::Type* getReferenceType(const AbstractClass* const& a){
		return a->type->getPointerTo();
	}
protected:
	ReferenceClass(const AbstractClass* const in):
		AbstractClass(nullptr,in->name+"&",nullptr,PRIMITIVE_LAYOUT,CLASS_REF,true,getReferenceType(in)),
		innerType(in)
		{
		assert(in->classType!=CLASS_REF);
		assert(in->classType!=CLASS_LAZY);
		assert(in->classType!=CLASS_VOID);
		///register methods such as print / tostring / tofile / etc
	}
public:

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		id.error("Cannot get local of reference");
		exit(1);
//		return innerType->getLocalReturnClass(id,s);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		id.error("Cannot get local of reference");
		exit(1);
		//return innerType->getLocalData(r, id, s, new )
		//fdasexit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		return toCast ==this || (toCast->classType==CLASS_REF && innerType->noopCast(
				((const ReferenceClass*)toCast)->innerType)) || toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	inline bool hasCast(const AbstractClass* const toCast) const{
		return noopCast(toCast);
	}
	inline llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		if(noopCast(toCast)){
			assert(toCast->type->isPointerTy());
			if(toCast->type==type) return valueToCast;
			else return r.pointerCast(valueToCast, (llvm::PointerType*) toCast->type);
		}
		id.error("Cannot cast reference");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return 0;
//		return innerType->compare(a,b);
	}
	static ReferenceClass* get(const AbstractClass* const arg) {
		static std::map<const AbstractClass*,ReferenceClass*> map;
		ReferenceClass*& fc = map[arg];
		if(fc==nullptr) fc = new ReferenceClass(arg);
		return fc;
	}
};



#endif /* REFERENCECLASS_HPP_ */
