/*
 * IntClass.hpp
 *
 *  Created on: Dec 26, 2013
 *      Author: Billy
 */

#ifndef INTCLASS_HPP_
#define INTCLASS_HPP_
#include "RealClass.hpp"
#include "../ScopeClass.hpp"
class IntClass: public RealClass{
public:
	IntClass(Scopable* s, String nam, unsigned len);
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
		return ((llvm::IntegerType*)type)->getBitWidth();
	}
	llvm::ConstantInt* getZero(PositionID id, bool negative=false) const override final{
		return llvm::ConstantInt::get((llvm::IntegerType*)type,(uint64_t)0);
	}
	llvm::ConstantInt* getOne(PositionID id) const override final{
		return llvm::ConstantInt::get((llvm::IntegerType*)type,(uint64_t)1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return (toCast->classType==CLASS_INT && type==toCast->type)|| toCast->classType==CLASS_VOID;
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
	inline void checkFit(PositionID id, int64_t const value) const{
		if(value<0){
			if((-value) >> getWidth() > 0) id.warning("Cannot fit negative integer literal in signed type");
		} else {
			if((value) >> getWidth() > 0) id.warning("Cannot fit positive integer literal in integral type");
		}
	}
	inline void checkFit(PositionID id, mpz_t const value) const{
		if(mpz_sgn(value)<0){
			auto t_width=mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.warning("Cannot fit negative integer literal needing "+str(t_width)+" bits in signed type of size "+str(r_width)+" bits");
		} else {
			auto t_width = mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.warning("Cannot fit positive integer literal needing "+str(t_width)+" bits in integral type of size "+str(r_width)+" bits");
		}
	}
	inline llvm::ConstantInt* getMaxValue () const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getSignedMaxValue(getWidth()));
	}
	inline llvm::ConstantInt* getMinValue () const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getSignedMinValue(getWidth()));
	}
	inline llvm::ConstantInt* getAllOnes() const{
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getAllOnesValue(getWidth()));
	}
	inline llvm::ConstantInt* getOneBitSet(unsigned BitNo) const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getOneBitSet(getWidth(),BitNo));
	}
	inline llvm::ConstantInt* getBitsSet(unsigned loBit, unsigned hiBit) const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getBitsSet(getWidth(),loBit,hiBit));
	}
	inline llvm::ConstantInt* getHighBitsSet( unsigned hiBit) const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getHighBitsSet(getWidth(),hiBit));
	}
	inline llvm::ConstantInt* getLowBitsSet( unsigned hiBit) const {
		return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::getLowBitsSet(getWidth(),hiBit));
	}
	inline llvm::ConstantInt* getValue(PositionID id, const int64_t value) const{
		checkFit(id,value);
		llvm::ConstantInt* ret = llvm::ConstantInt::get((llvm::IntegerType*)(type),value);
		return ret;
	}
	inline llvm::ConstantInt* getValue(PositionID id, const mpz_t& value) const override final{
		checkFit(id,value);
		char temp[mpz_sizeinbase (value, 10) + 2];
		auto tmp =  mpz_get_str(temp, 10, value);
		llvm::ConstantInt* ret = llvm::ConstantInt::get((llvm::IntegerType*)(type),llvm::StringRef(String(tmp)),10);
		return ret;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		if(toCast->layout!=PRIMITIVE_LAYOUT) return false;
		switch(toCast->classType){
		case CLASS_INT:{
			IntClass* nex = (IntClass*)toCast;
			return nex->getWidth()>=getWidth();
		}
		case CLASS_BIGINT:
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
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override;
};

const IntClass byteClass(nullptr, "byte", 8);
const IntClass shortClass(nullptr, "short", 16);
const IntClass intClass(nullptr, "int", 32);
const IntClass longClass(nullptr, "long", 64);

const IntClass c_intClass(&(NS_LANG_C.staticVariables), "int", 8*sizeof(int));
const IntClass c_longClass(&(NS_LANG_C.staticVariables), "long", 8*sizeof(long));
const IntClass c_longlongClass(&(NS_LANG_C.staticVariables), "long_long", 8*sizeof(long long));
const IntClass c_size_tClass(&(NS_LANG_C.staticVariables), "size_t", 8*sizeof(size_t));

#endif /* INTCLASS_HPP_ */
