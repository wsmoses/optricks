/*
 * NullClass.hpp
 *
 *  Created on: May 12, 2014
 *      Author: Billy
 */

#ifndef NULLCLASS_HPP_
#define NULLCLASS_HPP_
#include "../AbstractClass.hpp"

class NullClass: public AbstractClass{
public:
	NullClass(bool b): AbstractClass(nullptr,"null_t",nullptr,POINTER_LAYOUT,CLASS_NULL,true,VOIDTYPE){
		LANG_M.addClass(PositionID(0,0,"#null"),this);
	};
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
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		//TODO FOR REAL
		assert(a->classType==CLASS_NULL);
		assert(b->classType==CLASS_NULL);
		return 0;
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		//TODO FOR REAL
		return toCast->classType==CLASS_NULL;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		//TODO FOR REAL
		return toCast->classType==CLASS_NULL;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		//TODO FOR REAL
		if(toCast->classType!=CLASS_NULL) illegalCast(id,toCast);
		return valueToCast;
	}
};

const VoidClass voidClass(true);

#endif /* NULLCLASS_HPP_ */
