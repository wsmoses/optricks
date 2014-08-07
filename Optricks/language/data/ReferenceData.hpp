/*
 * ReferenceData.hpp
 *
 *  Created on: Mar 26, 2014
 *      Author: Billy
 */

#ifndef REFERENCEDATA_HPP_
#define REFERENCEDATA_HPP_
#include "./Data.hpp"
#include "../class/builtin/ReferenceClass.hpp"
class ReferenceData:public Data{
public:
	const LocationData* const value;
	ReferenceData(const LocationData* const val):
		Data(R_REF),value(val){
		assert(val);
	}
	const AbstractClass* getMyClass(PositionID id) const override final{
		id.error("Cannot use reference as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return ReferenceClass::get(value->type)->hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		//TODO fix
		const ReferenceClass* ra = (const ReferenceClass*)a;
		const ReferenceClass* rb = (const ReferenceClass*)b;
		return value->type->compare(ra, rb);
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* inst) const override final{
		id.error("Cannot call function of reference");
		exit(1);
	}
	const AbstractClass* getReturnType() const override final{
		return ReferenceClass::get(value->type);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b)const override final{
		id.error("Cannot call function of reference");
		exit(1);
	}

	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of reference");
		exit(1);
	}
	inline void setValue(RData& r, llvm::Value* v) const{
		PositionID(0,0,"#ref").compilerError("Cannot set value of reference");
		exit(1);
	}
	inline const ReferenceData* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(right==value->type) return this;
		else{
			//todo check
			llvm::Value* V = value->value->getPointer(r, id);
			assert(right->type->isPointerTy());
			if(V->getType()!=right->type) V = r.pointerCast(V, (llvm::PointerType*) right->type);
			return new ConstantData(V, right);
		}
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(hasCastValue(right)){
			llvm::Value* V = value->value->getPointer(r, id);
			assert(right->type->isPointerTy());
			if(V->getType()==right->type) return V;
			else return r.pointerCast(V, (llvm::PointerType*) right->type);
		}
		id.compilerError("Cannot cast reference V "+value->type->getName()+" to "+right->getName());
		exit(1);
	}
};



#endif /* REFERENCEDATA_HPP_ */
