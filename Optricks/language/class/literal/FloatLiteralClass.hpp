/*
 * FloatLiteralClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALCLASS_HPP_
#define FLOATLITERALCLASS_HPP_
#include "../AbstractClass.hpp"
#include "../builtin/RealClass.hpp"

class FloatLiteralClass: public RealClass{
public:
	FloatLiteralClass(bool b):
		RealClass("floatLiteral",LITERAL_LAYOUT,CLASS_FLOATLITERAL,llvm::IntegerType::get(getGlobalContext(), 1)){
#define FL(N,R,M) \
	LANG_M->addFunction(PositionID(0,0,"#float"),N)->add(\
		new BuiltinInlineFunction(new FunctionProto(N, {AbstractDeclaration(this)},R),\
			nullptr,[this](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> const Data*{\
		M;})\
		,PositionID(0,0,"#float"));
#define FLT(N,MP) FL(N,this,\
		assert(args.size()==1);\
		auto& F = ((const FloatLiteral*)(args[0]->evaluate(r)))->value;\
		auto ret = new FloatLiteral(0,0,0);\
		MP(ret->value, F, MPFR_RNDN);\
		return ret;\
);

		FL("isNan",&boolClass,
				assert(args.size()==1);
				auto& F = ((const FloatLiteral*)(args[0]->evaluate(r)))->value;
				return new ConstantData(BoolClass::getValue(mpfr_nan_p(F)),&boolClass)
		);
		FL("sgn",&intLiteralClass,
				assert(args.size()==1);
				auto& F = ((const FloatLiteral*)(args[0]->evaluate(r)))->value;
				auto tmp = mpfr_sgn(F);
				if(tmp==0) return &ZERO_LITERAL;
				if(tmp<0) return &MINUS_ONE_LITERAL;
				else return &ONE_LITERAL;
		);
		FLT("abs",mpfr_abs);

		FLT("log",mpfr_log);
		FLT("log2",mpfr_log2);
		FLT("log10",mpfr_log10);

		FLT("exp",mpfr_exp);
		FLT("exp2",mpfr_exp2);
		FLT("exp10",mpfr_exp10);

		FLT("sin",mpfr_sin);
		FLT("cos",mpfr_cos);
		FLT("tan",mpfr_tan);

		FLT("sec",mpfr_sec);
		FLT("csc",mpfr_csc);
		FLT("cot",mpfr_cot);

		FLT("acos",mpfr_acos);
		FLT("asin",mpfr_asin);
		FLT("atan",mpfr_atan);
		//TODO atan2

		FLT("sinh",mpfr_sinh);
		FLT("cosh",mpfr_cosh);
		FLT("tanh",mpfr_tanh);

		FLT("sech",mpfr_sech);
		FLT("csch",mpfr_csch);
		FLT("coth",mpfr_coth);

		FLT("asinh",mpfr_asinh);
		FLT("acosh",mpfr_acosh);
		FLT("atanh",mpfr_atanh);

		FLT("logp1",mpfr_log1p);
		FLT("expm1",mpfr_expm1);
		FLT("eint",mpfr_eint);
		FLT("li2",mpfr_li2);
		FLT("gamma",mpfr_gamma);
		FLT("lngamma",mpfr_lngamma);
		FLT("digamma",mpfr_digamma);
		FLT("zeta",mpfr_zeta);
		FLT("erf",mpfr_erf);
		FLT("erfc",mpfr_erfc);



#undef FLT
#undef FL
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_FLOATLITERAL: return true;
		case CLASS_RATIONAL:
		default:
			return false;
		}
	}

	AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		illegalLocal(id,s);
		exit(1);
	}
	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		illegalLocal(id,s);
		exit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(a->classType==CLASS_FLOAT || a==this );
		assert(b->classType==CLASS_FLOAT || b==this);
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
	}
	Constant* getZero(PositionID id, bool negative=false) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	Constant* getOne(PositionID id) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	Constant* getValue(PositionID id, const mpz_t& c) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
};

const FloatLiteralClass floatLiteralClass(true);
#endif /* FLOATLITERALCLASS_HPP_ */
