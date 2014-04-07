/*
 * BoolClass.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef BOOLCLASS_HPP_
#define BOOLCLASS_HPP_
#include "../AbstractClass.hpp"
#include "./VoidClass.hpp"
#include "./LazyClass.hpp"
class BoolClass: public AbstractClass{
public:
	inline BoolClass(bool b):
		AbstractClass(nullptr,"bool", nullptr,PRIMITIVE_LAYOUT,CLASS_BOOL,true,IntegerType::get(getGlobalContext(),1)){
		LANG_M->addClass(PositionID(0,0,"#int"),this);
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		illegalLocal(id,s);
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
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
	unsigned getWidth() const{
		return ((IntegerType*)type)->getBitWidth();
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(a->classType==CLASS_BOOL);
		assert(b->classType==CLASS_BOOL);
		return 0;
		/*
		if(a->classType==CLASS_BOOL)
			return (b->classType==CLASS_BOOL)?(0):(-1);
		else
			return (b->classType==CLASS_BOOL)?(1):(0);*/
	}
	inline static Constant* getValue(bool value){
		if(value)
			return ConstantInt::getTrue(BOOLTYPE);
		else
			return ConstantInt::getFalse(BOOLTYPE);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_BOOL;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_BOOL;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_BOOL) illegalCast(id,toCast);
		return valueToCast;
	}
};

const BoolClass boolClass(true);

#endif /* BOOLCLASS_HPP_ */
