/*
 * ClassFunctionData.hpp
 *
 *  Created on: Mar 3, 2014
 *      Author: Billy
 */

#ifndef CLASSFUNCTIONDATA_HPP_
#define CLASSFUNCTIONDATA_HPP_

#include "Data.hpp"
#include "./VoidData.hpp"
#include "../../operators/LocalFuncs.hpp"
class ClassFunctionData:public Data{
public:
	const Data* const instance;
	const String function;
	ClassFunctionData(const Data* i, const String f):Data(R_CLASSFUNC),instance(i),function(f){};
	const AbstractClass* getReturnType() const override final{
		PositionID(0,0,"#hm").compilerError("Cannot use instance.localFunction as wrapper");
		exit(1);
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		id.compilerError("Cannot cast class function closure");
		exit(1);
	}
	inline Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot cast class function closure");
		exit(1);
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	/*inline LocationData* toLocation(RData& r) const override final{
		//TODO complete ConstantData toLocation
		assert(0);
		cerr << "TupleData::toLocation has yet to be implemented" << endl << flush;
		exit(1);
	}*/
	inline Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		id.compilerError("Cannot cast class function closure");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		PositionID(0,0,"#classfunc").compilerError("Cannot cast class function closure");
		exit(1);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override {
		PositionID(0,0,"#classfunc").compilerError("Cannot compareValue class function closure");
		exit(1);
	}

	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override{
		return getLocalFunction(r, id, function, instance, args);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override {
		return getLocalFunctionReturnType(id, function, instance->getReturnType(), args);
	}
	/**
	 * Returns the class that this represents, if it represents a class
	 */
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.compilerError("Cannot use function closure as class");
		exit(1);
	}
};




#endif /* CLASSFUNCTIONDATA_HPP_ */
