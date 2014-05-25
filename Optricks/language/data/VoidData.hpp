/*
 * VoidData.hpp
 *
 *  Created on: Dec 30, 2013
 *      Author: Billy
 */

#ifndef VOIDDATA_HPP_
#define VOIDDATA_HPP_
#include "Data.hpp"
class VoidData: public Data{
public:
	inline VoidData(int a):Data(R_VOID){}

	bool hasCastValue(const AbstractClass* const a) const override final{
			return false;
		}
		int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
			return 0;
		}
	inline const AbstractClass* getReturnType() const override final{
		PositionID(0,0,"#void").compilerError("Cannot get returntype of void data");
		exit(1);
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		PositionID(0,0,"#void").compilerError("Cannot get castTo of void data");

		exit(1);
	}
	const Data* toValue(RData& r, PositionID id) const override final{
		return this;
	}
	llvm::Value* getValue(RData& r, PositionID id) const override final{
		id.error("Cannot getValue of void");
		exit(1);
	}
	llvm::Value* castToV(RData& r, const AbstractClass* const ac, PositionID id) const override final{
		id.error("Cannot castToV of void");
		exit(1);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("Void cannot act as function");
		exit(1);
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override{
		id.error("Cannot use void as function");
		return this;
	}
	/**
	 * Returns the class that this represents, if it represents a class
	 */
	AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use void as class");
		exit(1);
		//return voidClass;
	}
};

VoidData VOID_DATA(0);



#endif /* VOIDDATA_HPP_ */
