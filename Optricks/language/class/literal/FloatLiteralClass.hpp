/*
 * FloatLiteralClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALCLASS_HPP_
#define FLOATLITERALCLASS_HPP_
#include "../AbstractClass.hpp"
#include "../builtin/RealClass.hpp"

class FloatLiteralClass: public RealClass{
public:
	FloatLiteralClass(bool b);
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_VOID: return true;
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_FLOATLITERAL: return true;
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
		if(toCast->classType==CLASS_VOID) return true;
		return hasCast(toCast);
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;

		if(a->classType==CLASS_COMPLEX && b->classType==CLASS_COMPLEX)
			return compare(((const ComplexClass*)a)->innerClass, ((const ComplexClass*)b)->innerClass);
		else if(a->classType==CLASS_COMPLEX) return 1;
		else if(b->classType==CLASS_COMPLEX) return -1;
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
	}
	llvm::Constant* getZero(PositionID id, bool negative=false) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	llvm::Constant* getOne(PositionID id) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	llvm::Constant* getValue(PositionID id, const mpz_t& c) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
};

const FloatLiteralClass floatLiteralClass(true);
#endif /* FLOATLITERALCLASS_HPP_ */
