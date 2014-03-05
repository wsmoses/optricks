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
		assert(a->classType==CLASS_CLASS);
		assert(b->classType==CLASS_CLASS);
		return 0;
	}

	SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const override final;
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CLASS;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CLASS;
	}
	const AbstractClass* getReturnType() const override final{
		return this;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_CLASS) illegalCast(id,toCast);
		return valueToCast;
	}
};

ClassClass* classClass = new ClassClass(true);



#endif /* CLASSCLASS_HPP_ */
