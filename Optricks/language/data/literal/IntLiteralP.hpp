/*
 * IntLiteral.cpp
 *
 *  Created on: Dec 22, 2013
 *      Author: Billy
 */
#ifndef IntLiteralP_HPP_
#define IntLiteralP_HPP_
#include "IntLiteral.hpp"
#include "FloatLiteral.hpp"
#include "ComplexLiteral.hpp"
#include "RationalLiteral.hpp"
#include "../../class/builtin/IntClass.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/builtin/ComplexClass.hpp"
#include "../../class/builtin/RationalClass.hpp"
#include "../../class/literal/LiteralClass.hpp"

/*
const AbstractClass* IntLiteral::getReturnType() const{
	if(intType) return intType;
	else{
		return intLiteralClass;
	}
}

ConstantInt* IntLiteral::getValue(RData& r, PositionID id) const{
	if(intType==NULL){
		id.error("Cannot getValue of class without type");
		exit(1);
	} else {
		assert(value>=0 || intType->isSigned);
		assert((intType->isSigned)?(mpz_sizeinbase(value,2)+1):(mpz_sizeinbase(value,2)) >=intType->getWidth());
		auto tmp =  mpz_get_str(nullptr, 10, value);
		void (*freefunc)(void*,size_t);
		ConstantInt* ret = ConstantInt::get((IntegerType*)(intType->type),tmp,10);
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
		return ret;
	}
}
const Literal* IntLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
	case CLASS_FLOAT: return new FloatLiteral(value, (FloatClass*)right);
	case CLASS_COMPLEX: return new ComplexLiteral(this,nullptr,(ComplexClass*)right);
	case CLASS_INT:{
		IntClass* tmp = (IntClass*)right;
		if(tmp==intType) return this;
		else{
			if(intType==NULL) tmp->checkFit(id,value);
			else if(!intType->hasCast(right)) id.error("Cannot cast int literal of type "+intType->getName()+" to "+right->getName());
			return new IntLiteral(value, tmp);
		}
	}
	default:
		id.error("Cannot cast integer literal to non-integer type "+right->getName());
		exit(1);
	}
}
Constant* IntLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	switch(right->classType){
	case CLASS_FLOAT: return ((FloatClass*)right)->getValue(id, value);
	case CLASS_COMPLEX: return ((ComplexClass*)right)->getValue(id, value);
	case CLASS_INT: return ((IntClass*)right)->getValue(id, value);
	default:
		id.error("Cannot cast integer literal to non-integer type "+right->getName());
		exit(1);
		/*case CLASS_RATIONAL: {
		//TODO force alignment to class size if necessary
		if(type==NULL) ((RationalClass*)right)->checkFit(id,value);
		else if(!type->hasCast(right)) id.error("Cannot cast int literal of type "+type->name+" to "+right->name);
		return new RationalLiteral(value, 1, right);
	}*/
	}
}

bool IntLiteral::hasCastValue(const AbstractClass* const a) const{
	return (a->classType==CLASS_INT && ((IntClass*)a)->hasFit(value))
			|| a->classType==CLASS_FLOAT || (a->classType==CLASS_COMPLEX && ((ComplexClass*)a)->innerClass->classType==CLASS_FLOAT) ;
}

int IntLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	return getReturnType()->compare(a,b);
	assert(hasCastValue(a));
	assert(hasCastValue(b));
	if(a->classType==b->classType){
		if(a->classType==CLASS_COMPLEX){
			ComplexClass* ca = (ComplexClass*)a;
			assert(ca->innerClass->classType==CLASS_FLOAT || ca->innerClass->classType==CLASS_INT);
			ComplexClass* cb = (ComplexClass*)b;
			assert(cb->innerClass->classType==CLASS_FLOAT || cb->innerClass->classType==CLASS_INT);
			if(ca->innerClass->classType==cb->innerClass->classType){
				if(ca->innerClass->classType==CLASS_FLOAT){
					return 0;
					/*
						FloatClass* fa = (FloatClass*)(ca->innerClass);
						FloatClass* fb = (FloatClass*)(cb->innerClass);
						if(fa->floatType==fb->floatType) return 0;
						else return (fa->floatType < fb->floatType)?(-1):(1);
					 */
				} else {
					/*IntClass* ia = (IntClass*)(ca->innerClass);
					IntClass* ib = (IntClass*)(cb->innerClass);

					if(ia->getWidth()==ib->getWidth()) return 0;
					else return (ia->getWidth() < ib->getWidth())?(-1):(1);
					*/
				}
			} else {
				return (ca->innerClass->classType==CLASS_INT)?(-1):(1);
			}

		} else if(a->classType==CLASS_FLOAT){
			return 0;
			/*FloatClass* fa = (FloatClass*)a;
				FloatClass* fb = (FloatClass*)b;
				if(fa->floatType==fb->floatType) return 0;
				else return (fa->floatType < fb->floatType)?(-1):(1);*/
		} else {
			IntClass* ia = (IntClass*)(a);
			IntClass* ib = (IntClass*)(b);
			if(ia->getWidth()==ib->getWidth()) return 0;
			else return (ia->getWidth() < ib->getWidth())?(-1):(1);
		}
	} else{
		return (a->classType==CLASS_INT)?(-1):(
				(b->classType==CLASS_INT)?(1):(
						(a->classType==CLASS_FLOAT)?(-1):(1)
				)
		);
	}
}
*/
#endif
