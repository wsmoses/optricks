/*
 * ComplexLiteral.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef IMAGINARYLITERAL_HPP_
#define IMAGINARYLITERAL_HPP_


#include "Literal.hpp"
#include "../VoidData.hpp"
#include "./IntLiteral.hpp"
//TODO implement methods
class ImaginaryLiteral:public Literal{
public:
	const Literal* const real;
	const Literal* const imag;
	virtual ~ImaginaryLiteral(){};
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	ImaginaryLiteral(const Literal* r, const Literal* const i,String s=""):Literal(R_IMAG),real(r),imag(i){
		if(r){
			assert(real->type==R_INT || real->type==R_FLOAT);
		}
		assert(imag);
		assert(imag->type==R_INT || imag->type==R_FLOAT);
		if(s.length()>0) LANG_M.addVariable(PositionID(0,0,"#imag"),s,this);
		getReturnType();
	}
	const AbstractClass* getReturnType() const override final;
	llvm::ConstantVector* getValue(RData& r, PositionID id) const override final;
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	void toStream(ostream& s) const {
		if(real){
			if(imag->type==R_INT){
				IntLiteral* i = (IntLiteral*)real;
				if(mpz_sgn(i->value)!=0){
					char temp[mpz_sizeinbase (i->value, 10) + 2];
					auto tmp =  mpz_get_str(temp, 10, i->value);
					s << tmp;
					if( (imag->type==R_INT && mpz_sgn( ((IntLiteral*)imag)->value )>=0 )
						|| (imag->type==R_FLOAT && mpfr_sgn( ((FloatLiteral*)imag)->value )>=0 ) )
					s << "+";
				}
			} else if(imag->type==R_FLOAT){
				FloatLiteral* i = (FloatLiteral*)real;
				if(! mpfr_zero_p(i->value)){
					i->toStream(s);
					if( (imag->type==R_INT && mpz_sgn( ((IntLiteral*)imag)->value )>=0 )
						|| (imag->type==R_FLOAT && mpfr_sgn( ((FloatLiteral*)imag)->value )>=0 ) )
					s << "+";
				}
			}
		}
		if(imag->type==R_INT){
			IntLiteral* i = (IntLiteral*)imag;
			char temp[mpz_sizeinbase (i->value, 10) + 2];
			auto tmp =  mpz_get_str(temp, 10, i->value);
			s << tmp << "j";
		} else if(imag->type==R_FLOAT){
			FloatLiteral* i = (FloatLiteral*)imag;
			i->toStream(s);
			s << "j";
		}
	}
};

const ImaginaryLiteral MY_I(nullptr, & ONE_LITERAL,"I");
#endif /* COMPLEXLITERAL_HPP_ */
