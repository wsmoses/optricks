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
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		return this==toCast;
//		return innerType->hasCast(toCast);
	}

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
		return toCast ==this;
	}
	inline Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		if(toCast==this) return valueToCast;
		id.error("Cannot cast reference");
		exit(1);
		/////todo
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a==this && b==this);
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
