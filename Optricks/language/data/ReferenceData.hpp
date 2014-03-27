/*
 * ReferenceData.hpp
 *
 *  Created on: Mar 26, 2014
 *      Author: Billy
 */

#ifndef REFERENCEDATA_HPP_
#define REFERENCEDATA_HPP_
#include "./Data.hpp"
class ReferenceData:public Data{
public:
	LocationData* const value;
	ReferenceData(Location* const val):
		Data(R_REF),value(val){
		assert(val);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use reference as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return a->classType==CLASS_REF && value->type->noopCast(
				((const ReferenceClass*)a)->innerType);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		const ReferenceClass* ra = (const ReferenceClass*)a;
		const ReferenceClass* rb = (const ReferenceClass*)b;
		return value->type->compare(ra, rb);
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override final{
		id.error("Cannot call function of reference");
		exit(1);
	}
	const AbstractClass* getReturnType() const override final{
		return ReferenceClass::get(value->type);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		id.error("Cannot call function of reference");
		exit(1);
	}

	inline Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of reference");
		exit(1);
	}
	inline void setValue(RData& r, Value* v) const{
		PositionID(0,0,"#ref").compilerError("Cannot set value of reference");
		exit(1);
	}
	inline const ConstantData* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_REF && ((const ReferenceClass*)right)->innerType==value->type) return this;
		id.compilerError("Cannot cast reference");
		exit(1);
	}
	inline Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		id.compilerError("Cannot cast reference V");
		exit(1);
	}
};



#endif /* REFERENCEDATA_HPP_ */
