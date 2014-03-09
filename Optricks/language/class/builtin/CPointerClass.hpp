/*
 * CPointerClass.hpp
 *
 *  Created on: Jan 15, 2014
 *      Author: Billy
 */

#ifndef CPOINTERCLASS_HPP_
#define CPOINTERCLASS_HPP_

#include "../AbstractClass.hpp"
class CPointerClass: public AbstractClass{
public:
	inline CPointerClass(bool b):
		AbstractClass(nullptr,"c_pointer", NULL,PRIMITIVE_LAYOUT,CLASS_CPOINTER,true,C_POINTERTYPE){
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CPOINTER;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CPOINTER;
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
			assert(a->classType==CLASS_CPOINTER);
			assert(b->classType==CLASS_CPOINTER);
			return 0;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_CPOINTER) id.error("Cannot cast c_pointer type to "+toCast->getName());
		return valueToCast;
	}
};

CPointerClass* c_pointerClass = new CPointerClass(true);

#endif /* CPOINTERCLASS_HPP_ */
