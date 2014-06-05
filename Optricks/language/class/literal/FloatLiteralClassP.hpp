/*
 * FloatLiteralClassP.hpp
 *
 *  Created on: May 25, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALCLASSP_HPP_
#define FLOATLITERALCLASSP_HPP_
#include "./FloatLiteralClass.hpp"


	FloatLiteralClass::FloatLiteralClass(bool b):
		RealClass(nullptr,"floatLiteral",LITERAL_LAYOUT,CLASS_FLOATLITERAL,BOOLTYPE){
#define FL(N,R,M) \
	LANG_M.addFunction(PositionID(0,0,"#float"),N)->add(\
		new BuiltinInlineFunction(new FunctionProto(N, {AbstractDeclaration(this)},R),\
			[this](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> const Data*{\
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
		FL("abs2",&floatLiteralClass,
				assert(args.size()==1);
				auto& F = ((const FloatLiteral*)(args[0]->evaluate(r)))->value;
				FloatLiteral* f = new FloatLiteral(0,0,0);
				mpfr_mul(f->value, F, F, MPFR_RNDN);
				return f;
		);
		FLT("abs",mpfr_abs);
		FLT("sqrt",mpfr_sqrt);

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


#endif /* FLOATLITERALCLASSP_HPP_ */
