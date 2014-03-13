/*
 * ComplexClass.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef COMPLEXCLASS_HPP_
#define COMPLEXCLASS_HPP_

#include "../AbstractClass.hpp"
class ComplexClass: public AbstractClass{
public:
	const RealClass* innerClass;
	static llvm::Type* cType(const RealClass* inner){
		assert(inner);
		return inner->type;
	}
	inline ComplexClass(String name, const RealClass* inner):
		AbstractClass(nullptr,name, nullptr,PRIMITIVE_LAYOUT,CLASS_COMPLEX,true,VectorType::get(cType(inner),2)),innerClass(inner){
		assert(inner);
		assert(inner->classType!=CLASS_COMPLEX);
		assert(inner->classType==CLASS_INT || inner->classType==CLASS_FLOAT);
		assert(LANG_M);
		LANG_M->addClass(PositionID(0,0,"#int"),this);
	}
	inline Constant* getValue(PositionID id, mpfr_t const value) const{
		if(innerClass->classType!=CLASS_FLOAT) id.error("Cannot convert floating literal to "+getName());
		const FloatClass* in = (const FloatClass*)innerClass;
		SmallVector<Constant*,2> ar(2);
		ar[0] = in->getValue(id,value);
		ar[1] = innerClass->getZero(id);
		return ConstantVector::get(ar);
	}
	bool hasLocalData(String s) const override final{
		return s=="real" || s=="imag";
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		if(s!="real" && s!="imag"){
			illegalLocal(id,s);
			exit(1);
		} else return innerClass;
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final;
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_COMPLEX && innerClass->noopCast(((ComplexClass*)toCast)->innerClass);
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_COMPLEX && innerClass->hasCast(((ComplexClass*)toCast)->innerClass);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(a->classType==CLASS_COMPLEX);
		assert(b->classType==CLASS_COMPLEX);
		return innerClass->compare(((ComplexClass*)a)->innerClass, ((ComplexClass*)b)->innerClass);
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_COMPLEX) id.error("Cannot cast "+getName()+" type to "+toCast->getName());
		//if(toCast==this || innerClass->noopCast(((ComplexClass*)toCast)->innerClass)) return valueToCast;
		//if(!innerClass->hasCast(((ComplexClass*)toCast)->innerClass)) id.error()
		return innerClass->castTo(((ComplexClass*)toCast)->innerClass, r, id, valueToCast);
	}
	static inline ComplexClass* get(const RealClass* inner) {
		static std::map<const RealClass*, ComplexClass*> cache;
		auto found = cache.find(inner);
		if(found==cache.end()){
			assert(inner);
			ComplexClass* nex = new ComplexClass("complex{"+inner->getName()+"}",inner);
			cache.insert(std::pair<const RealClass*,ComplexClass*>(inner, nex));
			return nex;
		}
		else return found->second;
	}
	inline Constant* getZero(PositionID id, bool negative=false) const{
		return ConstantVector::getSplat(2, innerClass->getZero(id,negative));
	}
	inline Constant* getOne(PositionID id) const{
		SmallVector<Constant*,2> ar(2);
		ar[0] = innerClass->getOne(id);
		ar[1] = innerClass->getZero(id);
		return ConstantVector::get(ar);
	}
	inline Constant* getI(PositionID id) const{
		SmallVector<Constant*,2> ar(2);
		ar[0] = innerClass->getZero(id);
		ar[1] = innerClass->getOne(id);
		return ConstantVector::get(ar);
	}
	inline Constant* getValue(PositionID id, mpz_t const value) const{
		SmallVector<Constant*,2> ar(2);
		ar[0] = innerClass->getValue(id,value);
		ar[1] = innerClass->getZero(id);
		return ConstantVector::get(ar);
	}
};

ComplexClass* complexClass = new ComplexClass("complex",doubleClass);

#endif /* COMPLEXCLASS_HPP_ */
