/*
 * MathConstantLiteral.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef MATHCONSTANTLITERAL_HPP_
#define MATHCONSTANTLITERAL_HPP_

#include "Literal.hpp"
#include "../../class/literal/MathConstantClass.hpp"
class MathConstantLiteral:public Literal{
private:
public:
	/**
	 * If type is null, then this can be any long/integer type
	 */
	const MathConstantClass mathType;
	//IntLiteral(const mpz_t& val, IntClass* cp=NULL):Literal(R_INT),value(val),intType(cp){ assert(val);}
	MathConstantLiteral(MathConstant mt, String toAdd):Literal(R_MATH),
			mathType(mt){
		if(toAdd.length()>0) LANG_M.addVariable(PositionID(0,0,"#math"),toAdd,this);
		assert(mathType.mathType==mt);
	}
	const AbstractClass* getReturnType() const override final{
		return & mathType;
	}
	llvm::Value* getValue(RData& r, PositionID id) const override final{
		return llvm::UndefValue::get(mathType.type);
	}
	String toString() const{
		switch(mathType.mathType){
			case MATH_PI: return "Pi";
			case MATH_E: return "E";
			case MATH_EULER_MASC: return "EulerGamma";
			case MATH_LN2: return "Log2";
			case MATH_CATALAN: return "Catalan";
		}
		return "<error>";
	}
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(&mathType==right) return this;
		switch(right->classType){
		case CLASS_VOID: return &VOID_DATA;
		case CLASS_FLOAT:{
			llvm::Constant* cfp;
			const FloatClass* fc = (const FloatClass*)(right);
			switch(mathType.mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return new ConstantData(cfp, fc);
		}
		case CLASS_FLOATLITERAL:{
			auto F = new FloatLiteral(0,0,0);
			switch(mathType.mathType){
				case MATH_PI: mpfr_const_pi(F->value, MPFR_RNDN); break;
				case MATH_E:{
					mpfr_t ONE;
					mpfr_init(ONE);
					mpfr_set_ui(ONE, 1,MPFR_RNDN);
					mpfr_exp(F->value, ONE,MPFR_RNDN);
					break;
				}
				case MATH_EULER_MASC: mpfr_const_euler(F->value, MPFR_RNDN); break;
				case MATH_LN2: mpfr_const_log2(F->value, MPFR_RNDN); break;
				case MATH_CATALAN: mpfr_const_catalan(F->value, MPFR_RNDN); break;
			}
			return F;
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			if(cc->classType!=CLASS_FLOAT) id.error("Cannot cast math literal '"+mathType.getName()+"' to '"+cc->getName()+"'");
			llvm::Constant* cfp;
			const FloatClass* fc = (const FloatClass*)(cc->innerClass);
			switch(mathType.mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			llvm::SmallVector<llvm::Constant*,2> ar(2);
			ar[0] = cfp;
			ar[1] = cc->innerClass->getZero(id);
			return new ConstantData(llvm::ConstantVector::get(llvm::SmallVector<llvm::Constant*,2>(ar)),cc);
		}
		default:
			id.error("Math literal '"+mathType.getName()+"' cannot be cast to "+right->getName());
			exit(1);
		}
	}
	llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right==&mathType)
			return llvm::UndefValue::get(mathType.type);
		switch(right->classType){
		case CLASS_FLOAT:{
			llvm::Constant* cfp;
			const FloatClass* fc = (const FloatClass*)(right);
			switch(mathType.mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return cfp;
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			if(cc->classType!=CLASS_FLOAT) id.error("Cannot cast math literal '"+mathType.getName()+"' to '"+cc->getName()+"'");
			llvm::Constant* cfp;
			const FloatClass* fc = (const FloatClass*)(cc->innerClass);
			switch(mathType.mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			llvm::SmallVector<llvm::Constant*,2> ar(2);
			ar[0] = cfp;
			ar[1] = cc->innerClass->getZero(id);
			return llvm::ConstantVector::get(llvm::SmallVector<llvm::Constant*,2>(ar));
		}
		default:
			id.error("Math literal '"+mathType.getName()+"' cannot be cast to "+right->getName());
			exit(1);
		}
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return mathType.hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		return mathType.compare(a,b);
	}
};
const MathConstantLiteral MY_PI(MATH_PI,"Pi");
const MathConstantLiteral MY_E(MATH_E,"E");
const MathConstantLiteral MY_EULER_MASC(MATH_EULER_MASC,"EulerGamma");
const MathConstantLiteral MY_LN2(MATH_LN2,"Log2");
const MathConstantLiteral MY_CATALAN(MATH_CATALAN,"Catalan");


#endif /* MATHCONSTANTLITERAL_HPP_ */
