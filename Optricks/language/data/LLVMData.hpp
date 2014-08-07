/*
 * LLVMData.hpp
 *
 *  Created on: Dec 29, 2013
 *      Author: Billy
 */

#ifndef LLVMDATA_HPP_
#define LLVMDATA_HPP_
#include "Data.hpp"
#include "../class/AbstractClass.hpp"
class LLVMData: public Data{
public:
	const AbstractClass* const type;
	LLVMData(DataType tp, const AbstractClass* const r);
//	virtual Location* getLocation() const=0;
	virtual const LocationData* toLocation(RData& r, String name) const=0;
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const override final;
	const AbstractClass* getReturnType() const override final{
		return type;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final;
};



#endif /* LLVMDATA_HPP_ */
