/*
 * WrapperClass.hpp
 *
 *  Created on: Jun 10, 2014
 *      Author: Billy
 */

#ifndef WRAPPERCLASS_HPP_
#define WRAPPERCLASS_HPP_

#include "../AbstractClass.hpp"

class WrapperClass: public AbstractClass{
public:
	WrapperClass(String name, llvm::Type* T): AbstractClass(nullptr,name,nullptr,PRIMITIVE_LAYOUT,CLASS_WRAPPER,true,T){
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
		assert(a->classType==CLASS_WRAPPER);
		assert(b->classType==CLASS_WRAPPER);
		return 0;
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_WRAPPER;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_WRAPPER;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType!=CLASS_WRAPPER) illegalCast(id,toCast);
		return valueToCast;
	}
};


#endif /* WRAPPERCLASS_HPP_ */
