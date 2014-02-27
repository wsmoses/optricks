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
	const bool isSigned;
	IntClass(String nam, unsigned len, bool b):
		RealClass(nam, CLASS_INT,IntegerType::get(getGlobalContext(),len)),isSigned(b){
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	unsigned getWidth() const{
		return ((IntegerType*)type)->getBitWidth();
	}
	ConstantInt* getZero(bool negative=false) const override final{
		return ConstantInt::get((IntegerType*)type,0);
	}
	ConstantInt* getOne() const override final{
		return ConstantInt::get((IntegerType*)type,1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_INT && isSigned==((IntClass*)toCast)->isSigned && type==toCast->type;
	}
	inline bool hasFit(mpz_t const value) const{
		if(mpz_sgn(value)<0){
			if(!isSigned) return false;
			auto t_width=mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) return false;
			return true;
		} else {
			auto t_width = (isSigned)?(mpz_sizeinbase(value,2)+1):(mpz_sizeinbase(value,2));
			auto r_width = getWidth();
			if(t_width > r_width) return false;
			return true;
		}
	}
	inline void checkFit(PositionID id, mpz_t const value) const{
		if(mpz_sgn(value)<0){
			if(!isSigned) id.error("Cannot cast negative integer literal to unsigned type");
			auto t_width=mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit negative integer literal needing "+str(t_width)+" bits in signed type of size "+str(r_width)+" bits");
		} else {
			auto t_width = (isSigned)?(mpz_sizeinbase(value,2)+1):(mpz_sizeinbase(value,2));
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit positive integer literal needing "+str(t_width)+" bits in integral type of size "+str(r_width)+" bits");
			//TODO force APInt to be right width/sign for value
		}
	}
	inline ConstantInt* getMaxValue () const {
		if(isSigned) return ConstantInt::get(getGlobalContext(), APInt::getSignedMaxValue(getWidth()));
		else return ConstantInt::get(getGlobalContext(), APInt::getMaxValue(getWidth()));
	}
	inline ConstantInt* getMinValue () const {
		if(isSigned) return ConstantInt::get(getGlobalContext(), APInt::getSignedMinValue(getWidth()));
		else return ConstantInt::get(getGlobalContext(), APInt::getMinValue(getWidth()));
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
		ConstantInt* ret = ConstantInt::get((IntegerType*)(type),tmp,10);
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
		return ret;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->layout!=PRIMITIVE_LAYOUT) return false;
		switch(toCast->classType){
		case CLASS_INT:{
			IntClass* nex = (IntClass*)toCast;
			if(isSigned && !nex->isSigned) return false;
			auto n_width = nex->getWidth();
			auto s_width = getWidth();
			if(n_width>s_width) return true;
			else if(n_width<s_width) return false;
			else return isSigned==nex->isSigned;
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

IntClass* uint8Class = new IntClass("uint8", 8, false);
IntClass* uint16Class = new IntClass("uint16", 16, false);
IntClass* uint32Class = new IntClass("uint32", 32, false);
IntClass* uint64Class = new IntClass("uint64", 64, false);

IntClass* int8Class = new IntClass("int8",8, true);
IntClass* int16Class = new IntClass("int16", 16, true);
IntClass* int32Class = new IntClass("int32", 32, true);
IntClass* int64Class = new IntClass("int64", 64, true);

IntClass* byteClass = new IntClass("byte", 8, false);
IntClass* shortClass = new IntClass("short", 16, true);
IntClass* intClass = new IntClass("int", 32, true);
IntClass* longClass = new IntClass("long", 32, true);

IntClass* c_intClass = new IntClass("c_int", 8*sizeof(int),true);
IntClass* c_longClass = new IntClass("c_long", 8*sizeof(long),true);
IntClass* c_longlongClass = new IntClass("c_longlong", 8*sizeof(long long),true);
IntClass* c_size_tClass = new IntClass("c_size_t", 8*sizeof(size_t),true);

#endif /* INTCLASS_HPP_ */
