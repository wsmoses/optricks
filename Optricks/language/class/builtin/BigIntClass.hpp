/*
 * BigIntClass.hpp
 *
 *  Created on: Jul 8, 2014
 *      Author: Billy
 */

#ifndef BIGINTCLASS_HPP_
#define BIGINTCLASS_HPP_

#include "RealClass.hpp"
#include "../ScopeClass.hpp"
class BigIntClass: public RealClass{

static inline llvm::Type* getBigIntType(){
	llvm::SmallVector<llvm::Type*,2> ar(2);
	ar[0] = /* Counts (for garbage collection) */ intClass.type;
	ar[1] = /* Actual data */ llvm:ArrayType::get(CHARTYPE, sizeof(mpz_t));
	return llvm::PointerType::getUnqual(llvm::StructType::get(llvm::getGlobalContext(), ar,false));
}

public:
	BigIntClass(unsigned):RealClass(nullptr, "bigint", POINTER_LAYOUT,CLASS_BIGINT,
			getBigIntType()){
LANG_M.addClass(PositionID(0,0,"#int"),this);
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
/*
LANG_M.addFunction(PositionID(0,0,"#int"),"print")->add(
	new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
	[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
	assert(args.size()==1);
	Value* V = args[0]->evalV(r,id);
	auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
	//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
	if(auto C = dyn_cast<ConstantInt>(V)){
		String S = C->getValue().toString(10,true);
		auto F = S.length();
		for(unsigned i = 0; i<F; i++){
			r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type,S[i],false));
		}
		return &VOID_DATA;
	}

	char temp[mpz_sizeinbase (value, 10) + 2];
	mpz_get_str(temp, 10, value);
	auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
	//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
	for(const char* T = temp; *T !='\0'; ++T){
		r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
	}
	return &VOID_DATA;}), PositionID(0,0,"#int"));
LANG_M.addFunction(PositionID(0,0,"#int"),"println")->add(
	new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
	[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
	assert(args.size()==1);
	const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
	char temp[mpz_sizeinbase (value, 10) + 2];
	mpz_get_str(temp, 10, value);
	auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
	//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
	for(const char* T = temp; *T !='\0'; ++T){
		r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
	}
	r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, '\n',false));
	return &VOID_DATA;}), PositionID(0,0,"#int"));
*/


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
	llvm::ConstantInt* getZero(PositionID id, bool negative=false) const override final{
		return llvm::ConstantInt::get((llvm::IntegerType*)type,(uint64_t)0);
	}
	llvm::ConstantInt* getOne(PositionID id) const override final{
		return llvm::ConstantInt::get((llvm::IntegerType*)type,(uint64_t)1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return (toCast->classType==CLASS_BIGINT && type==toCast->type)|| toCast->classType==CLASS_VOID;
	}
	inline bool hasFit(mpz_t const value) const{
		return true;
	}
	inline void checkFit(PositionID id, int64_t const value) const{
	}
	inline void checkFit(PositionID id, mpz_t const value) const{
	}
	inline llvm::ConstantInt* getValue(PositionID id, const int64_t value) const{
		llvm::ConstantInt* ret = llvm::ConstantInt::get((llvm::IntegerType*)(type),value);
		return ret;
	}
	inline llvm::ConstantInt* getValue(PositionID id, const mpz_t& value) const override final{
		char temp[mpz_sizeinbase (value, 10) + 2];
		auto tmp =  mpz_get_str(temp, 10, value);
		llvm::ConstantInt* ret = llvm::ConstantInt::get((llvm::IntegerType*)(type),llvm::StringRef(String(tmp)),10);
		return ret;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		if(toCast->classType==CLASS_BIGINT) return true;
		/*if(toCast->layout!=PRIMITIVE_LAYOUT) return false;
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
		}*/
		return false;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final;
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override;
};

const BigIntClass bigIntClass();



#endif /* BIGINTCLASS_HPP_ */
