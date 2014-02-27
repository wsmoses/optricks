/*
 * FloatLiteral.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERAL_HPP_
#define FLOATLITERAL_HPP_
#include "Literal.hpp"
#include "../VoidData.hpp"
class FloatLiteral:public Literal{
public:
	enum FloatValueType{
			FLOAT_PI,
			FLOAT_E,
			FLOAT_LN2,
			FLOAT_CATALAN,
			FLOAT_NORMAL
		};

	FloatValueType floatValueType;
	mutable mpfr_t value;
	/**
	 * If type is null, then this can be any long/integer type
	 */
	FloatClass* const floatType;
	FloatLiteral(const char* str, unsigned base, FloatClass* cp=NULL):
		Literal(R_FLOAT),floatValueType(FLOAT_NORMAL),value(),floatType(cp){
		mpfr_init_set_str(value,str,base,MPFR_RNDN);
	}
	FloatLiteral(mpfr_t m, FloatValueType fvt,FloatClass* cp=nullptr):
		Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
		mpfr_init_set(value, m, MPFR_RNDN);
	}
	FloatLiteral(FloatValueType fvt,FloatClass* cp=nullptr):
		Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
	}
	FloatLiteral(double d, FloatValueType fvt=FLOAT_NORMAL, FloatClass* cp=NULL):
			Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
			mpfr_init_set_d(value,d,MPFR_RNDN);
	}
	FloatLiteral(long double d, FloatValueType fvt=FLOAT_NORMAL, FloatClass* cp=NULL):
			Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
			mpfr_init_set_ld(value,d,MPFR_RNDN);
	}
	FloatLiteral(char* str, unsigned base, FloatValueType fvt=FLOAT_NORMAL, FloatClass* cp=NULL):
			Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
			mpfr_init_set_str(value,str,base,MPFR_RNDN);
	}
	/*FloatLiteral(const mpfr_t& val, FloatValueType fvt=FLOAT_NORMAL, FloatClass* cp=NULL):
			Literal(R_FLOAT),floatValueType(fvt),value(),floatType(cp){
			mpfr_init(value);
			mpfr_set(value, val, MPFR_RNDN);
	}*/
	FloatLiteral(mpz_t const val, FloatClass* cp=NULL):
		Literal(R_FLOAT),floatValueType(FLOAT_NORMAL),value(),floatType(cp){
		mpfr_init(value);
		mpfr_set_z(value, val, MPFR_RNDN);
	}
	const AbstractClass* getReturnType() const override final;
	Constant* getValue(RData& r, PositionID id) const override final;

	const Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override{
		id.error("Cannot use floating-point literal as function");
		return VOID_DATA;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		id.error("Floating-point literal cannot act as function");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use floating-point literal as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final;

	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	const Literal* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	virtual ~FloatLiteral(){
		mpfr_clear(value);
	}
	void write(ostream& s, String t="") const override final{
		if(mpfr_regular_p(value)){
			//hard part
			assert(0);

		    char *st = NULL;
		    std::string out;

			if(!(mpfr_asprintf(&st,"%.RNE",value) < 0))
			{
				s << st;
				mpfr_free_str(st);
			} else assert(0);
		}
		else if(mpfr_nan_p(value)){
			if(floatType==nullptr) s << "nan";
			else s << floatType->getName() << "(nan)";
		}
		else if(mpfr_inf_p(value)){
			auto sb=mpfr_signbit(value);
			if(floatType==nullptr){
				if(sb) s << "-inf";
				else s << "inf";
			} else {
				if(sb) s << floatType->getName() << "(-inf)";
				else s << floatType->getName() << "(inf)";
			}
		} else{
			auto sb=mpfr_signbit(value);
			if(floatType==nullptr){
				if(sb) s << "-0.";
				else s << "0.";
			} else {
				if(sb) s << floatType->getName() << "(-0.)";
				else s << floatType->getName() << "(0.)";
			}
		}
	}
};





#endif /* FLOATLITERAL_HPP_ */
