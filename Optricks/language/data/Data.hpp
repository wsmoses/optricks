/*
 * DATA.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include "../includes.hpp"
#include "../evaluatable/Evaluatable.hpp"
#include "../location/Location.hpp"

#define DATA_C_
class Data: public Evaluatable{
public:
	const DataType type;
		Data(const DataType t):type(t){}
	public:
		virtual ~Data(){};
		inline const Data* evaluate(RData& r) const override{
			return this;
		}
		virtual const Data* toValue(RData& r, PositionID id) const=0;
		virtual const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const=0;
		virtual const AbstractClass* getReturnType() const=0;
		virtual Value* getValue(RData& r, PositionID id) const=0;
		virtual Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const=0;
		llvm::Value* evalV(RData& r,PositionID id) const override final{
			return getValue(r,id);
		}
		virtual const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const =0;
		/**
		 * Returns the class that this represents, if it represents a class
		 */
		virtual const AbstractClass* getMyClass(RData& r, PositionID id)const =0;
};

#endif /* DATA_HPP_ */
