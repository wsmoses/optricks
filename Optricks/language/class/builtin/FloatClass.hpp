/*
 * FloatClass.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef FLOATCLASS_HPP_
#define FLOATCLASS_HPP_
#include "RealClass.hpp"
#include "BoolClass.hpp"
#define FLOATCLASS_C_
class FloatClass: public RealClass{
public:
	enum FloatType{
		HalfTy=1, // 16-bit floating point type
		FloatTy=2, // 32-bit floating point type
		DoubleTy=3, // 64-bit floating point type
		X86_FP80Ty=4, // 80-bit floating point type (X87)
		FP128Ty=5,// 128-bit floating point type (112-bit mantissa)
		PPC_FP128Ty=6// 128-bit floating point type (two 64-bits, PowerPC)
	};
	const FloatType floatType;
	inline FloatClass(Scopable* s, String nam, FloatType t):
		RealClass(s, nam, PRIMITIVE_LAYOUT,CLASS_FLOAT,
				(t==HalfTy)?llvm::Type::getHalfTy(llvm::getGlobalContext()):(
				(t==FloatTy)?llvm::Type::getFloatTy(llvm::getGlobalContext()):(
				(t==DoubleTy)?llvm::Type::getDoubleTy(llvm::getGlobalContext()):(
				(t==X86_FP80Ty)?llvm::Type::getX86_FP80Ty(llvm::getGlobalContext()):(
				(t==FP128Ty)?llvm::Type::getFP128Ty(llvm::getGlobalContext()):(
				/*t==PPC_FP128Ty*/llvm::Type::getPPC_FP128Ty(llvm::getGlobalContext())
				)))))
	),floatType(t){
		(s?s:((Scopable*)(&LANG_M)))->addClass(PositionID(0,0,"#float"),this);


		LANG_M.addFunction(PositionID(0,0,"#float"),"abs2")->add(
			new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},this),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* V = args[0]->evalV(r, id);
			V = r.builder.CreateFMul(V, V);
			return new ConstantData(V, this);
		}), PositionID(0,0,"#float"));
#define SINGLE_FUNC_DECLR(X,Y) LANG_M.addFunction(PositionID(0,0,"#float"), X)->add(new IntrinsicFunction<llvm::Intrinsic::Y>(new FunctionProto(X,{AbstractDeclaration(this)},this)), PositionID(0,0,"#float"));
		SINGLE_FUNC_DECLR("abs",fabs)
		SINGLE_FUNC_DECLR("sqrt",sqrt)
		SINGLE_FUNC_DECLR("sin",sin)
		SINGLE_FUNC_DECLR("cos",cos)
		SINGLE_FUNC_DECLR("exp",exp)
		SINGLE_FUNC_DECLR("exp2",exp2)
		SINGLE_FUNC_DECLR("log",log)
		SINGLE_FUNC_DECLR("log2",log2)
		SINGLE_FUNC_DECLR("log10",log10)
		SINGLE_FUNC_DECLR("floor",floor)
		SINGLE_FUNC_DECLR("ceil",ceil)
		SINGLE_FUNC_DECLR("trunc",trunc)
		SINGLE_FUNC_DECLR("rint",rint)

		SINGLE_FUNC_DECLR("nearbyint",nearbyint)
		//SINGLE_FUNC_DECLR("round",round)


#undef SINGLE_FUNC_DECLR
		LANG_M.addFunction(PositionID(0,0,"#float"),"isNan")->add(
				new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				auto V = args[0]->evalV(r, id);
				return new ConstantData(r.builder.CreateFCmpUNO(V,V),&boolClass);}), PositionID(0,0,"#float"));


		LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
						new BuiltinInlineFunction(
								new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						llvm::Value* V=args[0]->evalV(r, id);
						if(t==FloatTy) V = r.builder.CreateFPExt(V, llvm::Type::getDoubleTy(llvm::getGlobalContext()));
						r.printf( (t<=DoubleTy)?"%f":"%Lf", V);
						return &VOID_DATA;
					}), PositionID(0,0,"#float"));
		LANG_M.addFunction(PositionID(0,0,"#str"),"println")->add(
						new BuiltinInlineFunction(
								new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						llvm::Value* V=args[0]->evalV(r, id);
						if(t==FloatTy) V = r.builder.CreateFPExt(V, llvm::Type::getDoubleTy(llvm::getGlobalContext()));
						r.printf( (t<=DoubleTy)?"%f\n":"%Lf\n", V);
						return &VOID_DATA;
					}), PositionID(0,0,"#float"));
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
	/*
	inline void checkFit(PositionID id, mpz_class const value) const{
		if(sgn(value)<0){
			if(!isSigned) id.error("Cannot cast negative integer literal to unsigned type");
			auto t_width=mpz_sizeinbase(value.get_mpz_t(),2)+1;
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit negative integer literal needing "+t_width+" bits in signed type of size "+r_width+" bits");
		} else {
			auto t_width = (isSigned)?(mpz_sizeinbase(value.get_mpz_t(),2)+1):(mpz_sizeinbase(value.get_mpz_t(),2));
			auto r_width = getWidth();
			if(t_width > r_width) id.error("Cannot fit positive integer literal needing "+t_width+" bits in integral type of size "+r_width+" bits");
			//TODO force APInt to be right width/sign for value
		}
	}*/
	inline const llvm::fltSemantics* getSemantics() const{
		switch(floatType){
		case HalfTy: return &llvm::APFloat::IEEEhalf;
		case FloatTy: return &llvm::APFloat::IEEEsingle;
		case DoubleTy: return &llvm::APFloat::IEEEdouble;
		case X86_FP80Ty: return &llvm::APFloat::x87DoubleExtended;
		case FP128Ty: return &llvm::APFloat::IEEEquad;
		case PPC_FP128Ty: return &llvm::APFloat::PPCDoubleDouble;
		default:
			assert(0);
			return &llvm::APFloat::Bogus;
		}
	}
	inline unsigned getWidth() const{
		unsigned r;
		switch(floatType){
		case HalfTy: r=16; break;
		case FloatTy: r=32;break;
		case DoubleTy: r=64;break;
		case X86_FP80Ty: r=80;break;
		case FP128Ty: r=128;break;
		case PPC_FP128Ty: r=128;break;
		//default: assert(0); return 0;
		}
		return r;
	}
	inline llvm::ConstantFP* getLargest (bool Negative=false) const {
		return llvm::ConstantFP::get(llvm::getGlobalContext(),llvm::APFloat::getLargest(*getSemantics(),Negative));
	}
	inline llvm::ConstantFP* getSmallest (bool Negative=false) const {
		return llvm::ConstantFP::get(llvm::getGlobalContext(),llvm::APFloat::getSmallest(*getSemantics(),Negative));
	}
	inline llvm::ConstantFP* getSmallestNormalized (bool Negative=false) const {
		return llvm::ConstantFP::get(llvm::getGlobalContext(),llvm::APFloat::getSmallestNormalized(*getSemantics(),Negative));
	}
	inline llvm::ConstantFP* getNaN() const{
		return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat::getNaN(*getSemantics()));
	}
	inline llvm::ConstantFP* getZero(PositionID id, bool negative=false) const override final{
		assert(this);
		return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat::getZero(*getSemantics(),negative));

		/*const llvm::fltSemantics& M = (this->getSemantics());
		//cerr << M << endl << flush;
		llvm::APFloat val = llvm::APFloat::getZero(M,negative);
		return llvm::ConstantFP::get(llvm::getGlobalContext(), val);*/
	}
	inline llvm::ConstantFP* getOne(PositionID id) const override final{
		return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(*getSemantics(),1));
	}
	inline llvm::Constant* getEulerMasc(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_euler(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline llvm::Constant* getPi(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_pi(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline llvm::Constant* getE(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_t ze;
		mpfr_init2(ze,getWidth());
		mpfr_set_ui(ze,1,MPFR_RNDN);
		mpfr_exp(e,ze,MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline llvm::Constant* getLN2(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_log2(e, MPFR_RNDN);
		auto tmp = getValue(id,e);
		mpfr_clear(e);
		return tmp;
	}
	inline llvm::Constant* getCatalan(PositionID id) const{
		mpfr_t e;
		mpfr_init2(e, getWidth());
		mpfr_const_catalan(e, MPFR_RNDN);
		auto tmp = getValue(id, e);
		mpfr_clear(e);
		return tmp;
	}
	inline llvm::Constant* getInfinity(bool negative=false) const{
		return llvm::ConstantFP::getInfinity(type,negative);
	}
	inline llvm::Constant* getValue(PositionID id, const mpz_t& value) const override final{
		char temp[mpz_sizeinbase (value, 10) + 2];
		mpz_get_str(temp, 10, value);
		return llvm::ConstantFP::get(type,String(temp));
	}
	inline llvm::Constant* getValue(PositionID id, const mpfr_t& value) const{
		if(mpfr_regular_p(value)){

		    char *s = NULL;
		    std::string out;

			if(!(mpfr_asprintf(&s,"%.RNE",value) < 0))
			{
				out = std::string(s);

				mpfr_free_str(s);
			} else id.compilerError("Error creating string for float to llvm conversion");
			return llvm::ConstantFP::get(llvm::getGlobalContext(),llvm::APFloat(*getSemantics(),out));
		}
		else if(mpfr_nan_p(value)) return getNaN();
		else if(mpfr_inf_p(value)){
			return getInfinity(mpfr_signbit(value));
		} else{
			assert(mpfr_zero_p(value));
			return getZero(id,mpfr_signbit(value));
		}
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return (toCast->classType==CLASS_FLOAT && type==toCast->type)|| toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override;
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final;
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override;
};

const FloatClass float16Class(nullptr, "float16", FloatClass::HalfTy);
const FloatClass float32Class(nullptr, "float32", FloatClass::FloatTy);
const FloatClass float64Class(nullptr, "float64", FloatClass::DoubleTy);
const FloatClass float80Class(nullptr, "float80", FloatClass::X86_FP80Ty);
const FloatClass float128Class(nullptr, "float128", FloatClass::FP128Ty);

const FloatClass floatClass(nullptr, "float", FloatClass::FloatTy);
const FloatClass doubleClass(nullptr, "double", FloatClass::DoubleTy);

const FloatClass c_floatClass(&(NS_LANG_C.staticVariables), "float", FloatClass::FloatTy);
const FloatClass c_doubleClass(&(NS_LANG_C.staticVariables), "double", FloatClass::DoubleTy);
#endif /* FLOATCLASS_HPP_ */
