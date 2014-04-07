/*
 * StringLiteralClass.hpp
 *
 *  Created on: Mar 25, 2014
 *      Author: Billy
 */

#ifndef STRINGLITERALCLASS_HPP_
#define STRINGLITERALCLASS_HPP_

#include "../AbstractClass.hpp"
#include "../builtin/VoidClass.hpp"
#include "./IntLiteralClass.hpp"

class StringLiteralClass: public AbstractClass{
public:
	StringLiteralClass(bool b);
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		if(toCast==this) return true;
		switch(toCast->classType){
		case CLASS_VOID: return true;
		case CLASS_STR:{
			PositionID(0,0,"#string").compilerError("Todo -- implement string");
			exit(1);
		}
		default:
			return false;
		}
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		if(a==this) return (b==this)?0:-1;
		else if(b==this) return 1;
		if(a->classType==CLASS_STR) return (b->classType==CLASS_STR)?0:-1;
		assert(0);
		return 0;
		/*
		if(a->classType==CLASS_COMPLEX) return (b->classType==CLASS_COMPLEX)?compare(((const ComplexClass*)a)->innerClass, ((const ComplexClass*)b)->innerClass):-1;
		else{
			assert(b->classType==CLASS_COMPLEX);
			return 1;
		}*/
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s=="length") return &intLiteralClass;
		illegalLocal(id,s);
		exit(1);
	}
	bool hasLocalData(String s) const override final{
		return s=="length";
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override;
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}
};

const StringLiteralClass stringLiteralClass(true);

#endif /* STRINGLITERALCLASS_HPP_ */
