/*
 * FloatLiteralClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALCLASS_HPP_
#define FLOATLITERALCLASS_HPP_
#include "../AbstractClass.hpp"

class FloatLiteralClass: public AbstractClass{
protected:
	FloatLiteralClass(bool b):
		AbstractClass(nullptr,"floatLiteral",nullptr,LITERAL_LAYOUT,CLASS_FLOATLITERAL,true,nullptr){
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_FLOATLITERAL: return toCast==this;
		case CLASS_RATIONAL:
		default:
			return false;
		}
	}

	AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		illegalLocal(id,s);
		exit(1);
	}
	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		illegalLocal(id,s);
		exit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(a->classType==CLASS_FLOAT || a==this );
		assert(b->classType==CLASS_FLOAT || b==this);
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
	}
	static FloatLiteralClass* get() {
		static FloatLiteralClass* fc = new FloatLiteralClass(true);
		return fc;
	}
};

#endif /* FLOATLITERALCLASS_HPP_ */
