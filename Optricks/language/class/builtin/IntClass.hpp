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
		LANG_M->addFunction(PositionID(0,0,"#float"),"isNan")->add(
						new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
								[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
						assert(args.size()==1);
						return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));

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
			if(t_width > r_width) id.warning("Cannot fit negative integer literal needing "+str(t_width)+" bits in signed type of size "+str(r_width)+" bits");
		} else {
			auto t_width = mpz_sizeinbase(value,2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.warning("Cannot fit positive integer literal needing "+str(t_width)+" bits in integral type of size "+str(r_width)+" bits");
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
	inline ConstantInt* getValue(PositionID id, const mpz_t& value) const override final{
		checkFit(id,value);
		char temp[mpz_sizeinbase (value, 10) + 2];
		auto tmp =  mpz_get_str(temp, 10, value);
		ConstantInt* ret = ConstantInt::get((IntegerType*)(type),StringRef(String(tmp)),10);
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

const IntClass byteClass("byte", 8);
const IntClass shortClass("short", 16);
const IntClass intClass("int", 32);
const IntClass longClass("long", 64);

const IntClass c_intClass("c_int", 8*sizeof(int));
const IntClass c_longClass("c_long", 8*sizeof(long));
const IntClass c_longlongClass("c_longlong", 8*sizeof(long long));
const IntClass c_size_tClass("c_size_t", 8*sizeof(size_t));

#endif /* INTCLASS_HPP_ */
