/*
 * IntClass.hpp
 *
 *  Created on: Dec 26, 2013
 *      Author: Billy
 */

#ifndef INTCLASS_HPP_
#define INTCLASS_HPP_
#include "RealClass.hpp"
class IntClass: public RealClass{
public:
	IntClass(String nam, unsigned len):
		RealClass(nam, PRIMITIVE_LAYOUT,CLASS_INT,IntegerType::get(getGlobalContext(),len)){
		LANG_M->addClass(PositionID(0,0,"#int"),this);
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
	unsigned getWidth() const{
		return ((IntegerType*)type)->getBitWidth();
	}
	ConstantInt* getZero(PositionID id, bool negative=false) const override final{
		return ConstantInt::get((llvm::IntegerType*)type,(uint64_t)0);
	}
	ConstantInt* getOne(PositionID id) const override final{
		return ConstantInt::get((llvm::IntegerType*)type,(uint64_t)1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_INT && type==toCast->type;
	}
	inline bool hasFit(mpz_t const value) const{
		if(mpz_sgn(value)<0){
			auto t_width=mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) return false;
			return true;
		} else {
			auto t_width = mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) return false;
			return true;
		}
	}
	inline void checkFit(PositionID id, mpz_t const value) const{
		if(mpz_sgn(value)<0){
			auto t_width=mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit negative integer literal needing "+str(t_width)+" bits in signed type of size "+str(r_width)+" bits");
		} else {
			auto t_width = mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit positive integer literal needing "+str(t_width)+" bits in integral type of size "+str(r_width)+" bits");
			//TODO force APInt to be right width/sign for value
		}
	}
	inline ConstantInt* getMaxValue () const {
		return ConstantInt::get(getGlobalContext(), APInt::getSignedMaxValue(getWidth()));
	}
	inline ConstantInt* getMinValue () const {
		return ConstantInt::get(getGlobalContext(), APInt::getSignedMinValue(getWidth()));
	}
	inline ConstantInt* getAllOnes() const{
		return ConstantInt::get(getGlobalContext(), APInt::getAllOnesValue(getWidth()));
	}
	inline ConstantInt* getOneBitSet(unsigned BitNo) const {
		return ConstantInt::get(getGlobalContext(), APInt::getOneBitSet(getWidth(),BitNo));
	}
	inline ConstantInt* getBitsSet(unsigned loBit, unsigned hiBit) const {
		return ConstantInt::get(getGlobalContext(), APInt::getBitsSet(getWidth(),loBit,hiBit));
	}
	inline ConstantInt* getHighBitsSet( unsigned hiBit) const {
		return ConstantInt::get(getGlobalContext(), APInt::getHighBitsSet(getWidth(),hiBit));
	}
	inline ConstantInt* getLowBitsSet( unsigned hiBit) const {
		return ConstantInt::get(getGlobalContext(), APInt::getLowBitsSet(getWidth(),hiBit));
	}
	inline ConstantInt* getValue(PositionID id, mpz_t const value) const override final{
		checkFit(id,value);
		auto tmp =  mpz_get_str(nullptr, 10, value);
		void (*freefunc)(void*,size_t);
		ConstantInt* ret = ConstantInt::get((IntegerType*)(type),StringRef(tmp),10);
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
		return ret;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->layout!=PRIMITIVE_LAYOUT) return false;
		switch(toCast->classType){
		case CLASS_INT:{
			IntClass* nex = (IntClass*)toCast;
			return nex->getWidth()>=getWidth();
		}
		case CLASS_FLOAT:
		case CLASS_RATIONAL:
		case CLASS_COMPLEX:
			return true;
		default:
			return false;
		}
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final;
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override;
};

IntClass* byteClass = new IntClass("byte", 8);
IntClass* shortClass = new IntClass("short", 16);
IntClass* intClass = new IntClass("int", 32);
IntClass* longClass = new IntClass("long", 64);

IntClass* c_intClass = new IntClass("c_int", 8*sizeof(int));
IntClass* c_longClass = new IntClass("c_long", 8*sizeof(long));
IntClass* c_longlongClass = new IntClass("c_longlong", 8*sizeof(long long));
IntClass* c_size_tClass = new IntClass("c_size_t", 8*sizeof(size_t));

#endif /* INTCLASS_HPP_ */
