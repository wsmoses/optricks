/*
 * ConstantData.hpp
 *
 *  Created on: Dec 20, 2013
 *      Author: Billy
 */

#ifndef CONSTANTDATA_HPP_
#define CONSTANTDATA_HPP_
#include "LLVMData.hpp"
#define CONSTANTDATA_C_
class ConstantData:public LLVMData{
public:
	llvm::Value* const value;
	ConstantData(llvm::Value* const val, const AbstractClass* const cp);
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		return value;
	}
	inline const ConstantData* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	const LocationData* toLocation(RData& r, String name) const override final;
	llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	static ConstantData* getTrue();
	static ConstantData* getFalse();
};

#endif /* CONSTANTDATA_HPP_ */
