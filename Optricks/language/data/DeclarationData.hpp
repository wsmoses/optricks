/*
 * DeclarationData.hpp
 *
 *  Created on: Apr 4, 2014
 *      Author: Billy
 */

#ifndef DECLARATIONDATA_HPP_
#define DECLARATIONDATA_HPP_
#include "../../ast/Declaration.hpp"

#define DECL_DATA_C_
class DeclarationData:public Data{
public:
	Declaration* const value;
	DeclarationData(Declaration* const val):
		Data(R_DEC),value(val){
		assert(val);
	}
	const AbstractClass* getMyClass(PositionID id) const override final{
		id.error("Cannot get class from declared variable");
		exit(1);
	}
	ReferenceData* toReference(RData& r) const{
		return new ReferenceData(value->finished);
	}
	inline void setValue(RData& r, llvm::Value* v) const{
		value->fastEvaluate(r);
		value->finished->setValue(r,v);
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return value->getReturnType()->hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		return value->getReturnType()->compare(a,b);
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const override final{
		value->fastEvaluate(r);
		return value->finished->callFunction(r, id, args, instance);
	}
	const AbstractClass* getReturnType() const override final{
		return value->getReturnType();
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		return value->getFunctionReturnType(id,args,isClassMethod);
	}

	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		return value->fastEvaluate(r)->getValue(r,id);
	}
	inline const Data* toValue(RData& r,PositionID id) const override final{
		value->fastEvaluate(r);
		return value->finished->toValue(r,id);
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		value->fastEvaluate(r);
		return value->finished->castTo(r, right,id);
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		value->fastEvaluate(r);
		return value->finished->castToV(r, right,id);
	}
};



#endif /* DECLARATIONDATA_HPP_ */
