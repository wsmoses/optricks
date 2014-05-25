/*
 * ClassClass.hpp
 *
 *  Created on: Jan 20, 2014
 *      Author: Billy
 */

#ifndef CLASSCLASS_HPP_
#define CLASSCLASS_HPP_

#include "../AbstractClass.hpp"
class ClassClass: public AbstractClass{
public:
	inline ClassClass(bool b):
		AbstractClass(nullptr,"Class", nullptr,PRIMITIVE_LAYOUT,CLASS_CLASS,true,CLASSTYPE){

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
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return 0;
	}

	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CLASS || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CLASS || toCast->classType==CLASS_VOID;
	}
	const AbstractClass* getReturnType() const override final{
		return this;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType!=CLASS_CLASS) illegalCast(id,toCast);
		return valueToCast;
	}
};

const ClassClass classClass(true);

const AbstractClass* AbstractClass::getReturnType() const{
	return &classClass;
}

const Data* AbstractClass::castTo(RData& r, const AbstractClass* const right, PositionID id) const {
	if(right->classType==CLASS_VOID) return &VOID_DATA;
	if(right->classType==CLASS_CLASS) return this;
	else{
		illegalCast(id, right);
		return this;
	}
}
#endif /* CLASSCLASS_HPP_ */
