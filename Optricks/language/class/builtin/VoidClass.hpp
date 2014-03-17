/*
 * VoidClass.hpp
 *
 *  Created on: Feb 7, 2014
 *      Author: Billy
 */

#ifndef VOIDCLASS_HPP_
#define VOIDCLASS_HPP_
#include "../AbstractClass.hpp"

class VoidClass: public AbstractClass{
public:
	VoidClass(bool b): AbstractClass(nullptr,"void",nullptr,LITERAL_LAYOUT,CLASS_VOID,true,VOIDTYPE){
		LANG_M->addClass(PositionID(0,0,"#int"),this);
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
		assert(a->classType==CLASS_VOID);
		assert(b->classType==CLASS_VOID);
		return 0;
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_VOID;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_VOID) illegalCast(id,toCast);
		return valueToCast;
	}
};

const VoidClass voidClass(true);

#endif /* VOIDCLASS_HPP_ */
