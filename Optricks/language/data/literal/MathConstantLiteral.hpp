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
	const MathConstantClass* mathType;
	//IntLiteral(const mpz_t& val, IntClass* cp=NULL):Literal(R_INT),value(val),intType(cp){ assert(val);}
	MathConstantLiteral(MathConstant mt):Literal(R_MATH),
			mathType(MathConstantClass::get(mt)){
	}
	const AbstractClass* getReturnType() const override final{
		return mathType;
	}
	Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of math literal");
		exit(1);
	}
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(mathType==right) return this;
		switch(right->classType){
		case CLASS_FLOAT:{
			ConstantFP* cfp;
			const FloatClass* fc = (const FloatClass*)(right);
			switch(mathType->mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return new ConstantData(cfp, fc);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			if(cc->classType!=CLASS_FLOAT) id.error("Cannot cast math literal '"+mathType->getName()+"' to '"+cc->getName()+"'");
			ConstantFP* cfp;
			const FloatClass* fc = (const FloatClass*)(cc->innerClass);
			switch(mathType->mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return new ConstantData(ConstantVector::get(llvm::SmallVector<Constant*,2>({cfp,fc->getZero(id)})),cc);
		}
		default:
			id.error("Math literal '"+mathType->getName()+"' cannot be cast to "+right->getName());
			exit(1);
		}
	}
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		switch(right->classType){
		case CLASS_FLOAT:{
			ConstantFP* cfp;
			const FloatClass* fc = (const FloatClass*)(right);
			switch(mathType->mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return new ConstantData(cfp, fc);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			if(cc->classType!=CLASS_FLOAT) id.error("Cannot cast math literal '"+mathType->getName()+"' to '"+cc->getName()+"'");
			ConstantFP* cfp;
			const FloatClass* fc = (const FloatClass*)(cc->innerClass);
			switch(mathType->mathType){
				case MATH_PI: cfp = fc->getPi(id); break;
				case MATH_E: cfp = fc->getE(id); break;
				case MATH_EULER_MASC: cfp = fc->getEulerMasc(id); break;
				case MATH_LN2: cfp = fc->getLN2(id); break;
				case MATH_CATALAN: cfp = fc->getCatalan(id); break;
			}
			return ConstantVector::get(llvm::SmallVector<Constant*,2>({cfp,fc->getZero(id)}));
		}
		default:
			id.error("Math literal '"+mathType->getName()+"' cannot be cast to "+right->getName());
			exit(1);
		}
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return mathType->hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		return mathType->compare(a,b);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("Math literal cannot act as function");
		exit(1);
	}
	virtual Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override final{
		id.error("Cannot call function on math literal");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use math literal as class");
		exit(1);
	}
};



#endif /* MATHCONSTANTLITERAL_HPP_ */
