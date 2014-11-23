/*
 * BigIntClass.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: Billy
 */

#ifndef BIGINTCLASS_HPP_
#define BIGINTCLASS_HPP_

#include "RealClass.hpp"
//#include "../../data/literal/IntLiteral.hpp"
#include "../ScopeClass.hpp"
#include "../literal/StringLiteralClass.hpp"
#include "./CPointerClass.hpp"
auto MPZ_TYPE = llvm::ArrayType::get(CHARTYPE, sizeof(mpz_t));
auto MPZ_POINTER = llvm::PointerType::getUnqual(MPZ_TYPE);

class BigIntClass: public AbstractClass{

static inline llvm::Type* getBigIntType(){
	llvm::SmallVector<llvm::Type*,2> ar(2);
	ar[0] = /* Counts (for garbage collection) */ intClass.type;
	ar[1] = /* Actual data */ MPZ_TYPE;
	return llvm::PointerType::getUnqual(llvm::StructType::get(llvm::getGlobalContext(), ar,false));
}

public:
	BigIntClass(unsigned):AbstractClass(nullptr, "bigint", nullptr, POINTER_LAYOUT,CLASS_BIGINT,true,
			getBigIntType()){
		LANG_M.addClass(PositionID(0,0,"#bigint"),this);
/*LANG_M.addFunction(PositionID(0,0,"#int"),"chr")->add(
		new BuiltinInlineFunction(new FunctionProto("chr",{AbstractDeclaration(this)},&charClass),
		[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		return new ConstantData(r.builder.CreateSExtOrTrunc(args[0]->evalV(r, id), CHARTYPE),&charClass);}), PositionID(0,0,"#float")
	);
LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
				new BuiltinInlineFunction(
						new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				//TODO have int32 / int64 / etc
				r.printf("%d", args[0]->evalV(r, id));
				return &VOID_DATA;
			}), PositionID(0,0,"#int"));
LANG_M.addFunction(PositionID(0,0,"#str"),"println")->add(
				new BuiltinInlineFunction(
						new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				//TODO have int32 / int64 / etc
				r.printf("%d\n", args[0]->evalV(r, id));
				return &VOID_DATA;
			}), PositionID(0,0,"#int"));
LANG_M.addFunction(PositionID(0,0,"#int"),"abs2")->add(
			new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},this),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* V = args[0]->evalV(r, id);
			V = r.builder.CreateMul(V, V);
			return new ConstantData(V, this);
}), PositionID(0,0,"#int"));
LANG_M.addFunction(PositionID(0,0,"#int"),"abs")->add(
			new BuiltinInlineFunction(new FunctionProto("abs",{AbstractDeclaration(this)},this),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			llvm::Value* V = args[0]->evalV(r, id);
			V = r.builder.CreateSelect(r.builder.CreateICmpSLT(V, this->getZero(id)), r.builder.CreateNeg(V),V);
			return new ConstantData(V, this);
}), PositionID(0,0,"#int"));

LANG_M.addFunction(PositionID(0,0,"#int"),"urem")->add(
			new BuiltinInlineFunction(new FunctionProto("urem",{AbstractDeclaration(this),AbstractDeclaration(this)},this),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==2);
			llvm::Value* V = args[0]->evalV(r, id);
			llvm::Value* V2 = args[1]->evalV(r, id);
			return new ConstantData(r.builder.CreateURem(V, V2), this);
}), PositionID(0,0,"#int"));
*/

		LANG_M.addFunction(PositionID(0,0,"#int"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			const Data* d = args[0]->evaluate(r);
			/*if(d->type==R_INT){
				const auto& value = ((const IntLiteral*) d)->value;

				char temp[mpz_sizeinbase (value, 10) + 2];
				mpz_get_str(temp, 10, value);
				auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
				//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
				for(const char* T = temp; *T !='\0'; ++T){
					r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
				}
			} else */{
				llvm::Value* V = d->getValue(r, id);
				//todo check for null?
				auto CU = r.getExtern("__gmp_printf", &stringLiteralClass, {}, true, "gmp");
				r.builder.CreateCall2(CU, r.getConstantCString("%Zd"), r.builder.CreateConstGEP2_32(V, 0, 1));
			}
			return &VOID_DATA;}), PositionID(0,0,"#int"));
LANG_M.addFunction(PositionID(0,0,"#int"),"println")->add(
	new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
	[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
	assert(args.size()==1);
	const Data* d = args[0]->evaluate(r);
	/*if(d->type==R_INT){
		const auto& value = ((const IntLiteral*) d)->value;

		char temp[mpz_sizeinbase (value, 10) + 2];
		mpz_get_str(temp, 10, value);
		auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
		//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
		for(const char* T = temp; *T !='\0'; ++T){
			r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
		}
		r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, '\n',false));
	} else */{
		llvm::Value* V = d->getValue(r, id);
		//todo check for null?
		auto CU = r.getExtern("__gmp_printf", &stringLiteralClass, {}, true, "gmp");
		r.builder.CreateCall2(CU, r.getConstantCString("%Zd\n"), r.builder.CreateConstGEP2_32(V, 0, 1));
	}

	return &VOID_DATA;}), PositionID(0,0,"#int"));
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		if(s=="_mpz")
			return &c_pointerClass;
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return s=="_mpz";
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		if(s=="_mpz"){
			return new ConstantData(r.builder.CreatePointerCast(r.builder.CreateConstGEP2_32(
					instance->getValue(r, id), 0, 1), C_POINTERTYPE), &c_pointerClass);
		}
		illegalLocal(id,s);
		exit(1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return (toCast->classType==CLASS_BIGINT && type==toCast->type)|| toCast->classType==CLASS_VOID;
	}
	inline bool hasFit(mpz_t const value) const{
		return true;
	}
	/*
	inline llvm::ConstantInt* getValue(PositionID id, const int64_t value) const{
		llvm::ConstantInt* ret = llvm::ConstantInt::get((llvm::IntegerType*)(type),value);
		return ret;
	}*/
	inline llvm::Value* getValue(RData& r, const mpz_t& value) const {
		llvm::Value* A = r.allocate(((llvm::PointerType*)type)->getElementType());
		//TODO reference counting
		r.builder.CreateStore(getInt32(0), r.builder.CreateConstGEP2_32(A, 0, 0));
		if(mpz_sgn(value)!=0){
			auto C_LONG = llvm::IntegerType::getIntNTy(llvm::getGlobalContext(), 8*sizeof(long int));
			if(mpz_fits_ulong_p(value)){
				llvm::SmallVector<llvm::Type*,1> args(2);
				args[0] = MPZ_POINTER;
				args[1] = C_LONG;
				auto CU = r.getExtern("__gmpz_init_set_ui",llvm::FunctionType::get(VOIDTYPE,args,false), "gmp");
				r.builder.CreateCall2(CU, (r.builder.CreateConstGEP2_32(A, 0, 1)), llvm::ConstantInt::get(C_LONG, (uint64_t)mpz_get_ui(value)));
			} else if(mpz_fits_slong_p(value)){
				llvm::SmallVector<llvm::Type*,1> args(2);
				args[0] = MPZ_POINTER;
				args[1] = C_LONG;
				auto CU = r.getExtern("__gmpz_init_set_si",llvm::FunctionType::get(VOIDTYPE,args,false), "gmp");
				r.builder.CreateCall2(CU, (r.builder.CreateConstGEP2_32(A, 0, 1)), llvm::ConstantInt::get(C_LONG, (int64_t)mpz_get_si(value)));
			} else {
				llvm::SmallVector<llvm::Type*,1> args(1);
				args[0] = MPZ_POINTER;
				auto CU = r.getExtern("__gmpz_init",llvm::FunctionType::get(VOIDTYPE,args,false), "gmp");
				llvm::Value* L;
				r.builder.CreateCall(CU, L=(r.builder.CreateConstGEP2_32(A, 0, 1)));
				size_t nail = 0;
				size_t size = sizeof(long);
				int numb = 8*size - nail;
				int count = (mpz_sizeinbase (value, 2) + numb-1) / numb;
				unsigned long p[count * size];
				size_t words_written;
				mpz_export (p, &words_written,1, size, 1, nail, value);
				assert(words_written <= count);
				llvm::SmallVector<unsigned long,0> vec(count);
				for(int i=0; i<count; i++)
					vec[i] = p[i];
				auto ar = llvm::ConstantDataArray::get(llvm::getGlobalContext(), vec);
				auto A = r.builder.CreateAlloca(ar->getType());
				r.builder.CreateStore(ar, A);

				//(mpz_t rop, size_t count, int order, size_t size, int endian, size_t nails, const void *op)
				llvm::SmallVector<llvm::Type*,7> args2(7);
				args2[0] = MPZ_POINTER;
				args2[1] = C_SIZETTYPE;
				args2[2] = C_INTTYPE;
				args2[3] = C_SIZETTYPE;
				args2[4] = C_INTTYPE;
				args2[5] = C_SIZETTYPE;
				args2[6] = C_POINTERTYPE;
				auto C = r.getExtern("__gmpz_import",llvm::FunctionType::get(VOIDTYPE,args2,false), "gmp");
				llvm::SmallVector<llvm::Value*,7> args3(7);
				args3[0] = L;
				args3[1] = getSizeT(count);
				args3[2] = getCInt(1);
				args3[3] = getSizeT(size);
				args3[4] = getCInt(1);
				args3[5] = getSizeT(nail);
				args3[6] = r.builder.CreatePointerCast(A, C_POINTERTYPE);

				r.builder.CreateCall(C, args3);
				if(mpz_sgn(value)<0){
					llvm::SmallVector<llvm::Type*,2> args4(2);
					args4[0] = MPZ_TYPE;
					args4[1] = MPZ_TYPE;
					auto C2 = r.getExtern("__gmpz_neg",llvm::FunctionType::get(VOIDTYPE,args4,false), "gmp");
					r.builder.CreateCall2(C2, L, L);
				}
			}
		} else {
			llvm::SmallVector<llvm::Type*,1> args(1);
			args[0] = MPZ_POINTER;
			auto CU = r.getExtern("__gmpz_init",llvm::FunctionType::get(VOIDTYPE,args,false), "gmp");
			r.builder.CreateCall(CU, (r.builder.CreateConstGEP2_32(A, 0, 1)));
		}
		return A;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		if(toCast->classType==CLASS_BIGINT) return true;
		//if(toCast->classType==CLASS_BIGFLOAT) return true;
		return false;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCast(a));
		assert(hasCast(b));

		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;

		if(a==this) return (b==this)?0:-1;
		else if(b==this) return 1;

		return 0;

	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		switch(toCast->classType){
			case CLASS_BIGINT:{
				return valueToCast;
			}
			default:
				id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
				exit(1);
		}
	}
};

const BigIntClass bigIntClass(0);



#endif /* BIGINTCLASS_HPP_ */
