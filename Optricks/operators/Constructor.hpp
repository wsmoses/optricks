/*
 * Constructor.hpp
 *
 *  Created on: Mar 12, 2014
 *      Author: Billy
 */

#ifndef CONSTRUCTOR_HPP_
#define CONSTRUCTOR_HPP_

#include "../language/includes.hpp"

const Data* AbstractClass::callFunction(RData& r, PositionID filePos, const std::vector<const Evaluatable*>& args, const Data* instance) const{
	assert(instance==nullptr);
	switch(classType){
	case CLASS_SCOPE:
		filePos.compilerError("Scope should never be instantiated");
		exit(1);
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:
	case CLASS_FUNC:
	case CLASS_VOID:
	case CLASS_CPOINTER:
	case CLASS_GEN:
	case CLASS_NULL:
	case CLASS_MAP:
	case CLASS_STRLITERAL:
	case CLASS_STR:
	case CLASS_CHAR:
	case CLASS_SET:
	case CLASS_CLASS:
	case CLASS_MATHLITERAL:
	case CLASS_FLOATLITERAL:
	case CLASS_INTLITERAL:
	case CLASS_RATIONAL:
	case CLASS_VECTOR:{
		filePos.error("Could not find constructor in class '"+getName()+"'");
		exit(1);
	}
	case CLASS_ARRAY:{
		if(args.size()!=1 ) filePos.error("Could not find valid constructor in array");
		auto L = args[0]->evaluate(r);
		auto V = L->getReturnType();
		llvm::Value* LEN;
		if(V->classType==CLASS_INT){
			llvm::Value* M = L->getValue(r, filePos);
			const IntClass* I = (const IntClass*)V;
			auto Im = I->getWidth();
			if(Im==32) LEN = M;
			else if(32>Im){
				LEN = r.builder.CreateSExt(M, type);
			} else{
				LEN = r.builder.CreateTrunc(M, type);
			}
		} else if(V->classType==CLASS_INTLITERAL){
			const IntLiteral* IL = (const IntLiteral*)L;
			assert(intClass.getWidth()==32);
			LEN = intClass.getValue(filePos, IL->value);
		}
		const ArrayClass* tc = (const ArrayClass*)this;
		uint64_t s = llvm::DataLayout(r.lmod).getTypeAllocSize(tc->inner->type);
		llvm::IntegerType* ic = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(size_t));
		llvm::Instruction* v = llvm::CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
				tc->inner->type, llvm::ConstantInt::get(ic, s), LEN);
		r.builder.Insert(v);
		/*
		 //TODO EMPTY
		 for(unsigned i = 0; i<inner.size(); i++){
			r.builder.CreateStore(inner[i]->castToV(r, tc->inner, id),
					r.builder.CreateConstGEP1_32(v, i));
		}*/
		assert(llvm::dyn_cast<llvm::PointerType>(tc->type));
		auto tmp=(llvm::StructType*)(((llvm::PointerType*)tc->type)->getElementType());
		s = llvm::DataLayout(r.lmod).getTypeAllocSize(tmp);
		llvm::Instruction* p = llvm::CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
						tmp, llvm::ConstantInt::get(ic, s));
		r.builder.Insert(p);
		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));
		r.builder.CreateStore(LEN,
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(LEN,
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		r.builder.CreateStore(v,G);
		return new ConstantData(p, this);
	}
	case CLASS_INT:{
		if(args.size()!=1 ) filePos.error("Could not find valid constructor in bool");
		const Data* d = args[0]->evaluate(r);
		auto V = d->getReturnType();
		const IntClass* T = (const IntClass*)this;
		if(V->classType==CLASS_STR){
			filePos.compilerError("Strings not implemented");
			exit(1);
		} else if(V->classType==CLASS_CHAR){
			auto M = d->getValue(r,filePos);
			if(auto D = llvm::dyn_cast<llvm::ConstantInt>(M)){
				if(D->getValue().ult('0') || D->getValue().ugt('9')){
					filePos.error("Character cannot be parsed as integer");
				}
				return new ConstantData(T->getValue(filePos,(D->getLimitedValue()-'0')), this);
			}
			filePos.compilerError("char parsing not implemented");
			exit(1);
		} else if(V->classType==CLASS_CSTRING){
			//TODO cstring parse int
			filePos.compilerError("parsing from cstring not implemented");
		} else if(V->classType==CLASS_STRLITERAL){
			auto M = ((const StringLiteral*)d)->value;
			mpz_t Z;
			mpz_init_set_str(Z,M.c_str(),10);
			auto tmp = new ConstantData(T->getValue(filePos, Z), this);
			mpz_clear(Z);
			return tmp;
		}
		else if(V->classType==CLASS_INT){
			llvm::Value* M = d->getValue(r, filePos);
			const IntClass* I = (const IntClass*)V;
			auto Im = I->getWidth();
			auto Tm = T->getWidth();
			if(Im==Tm) return new ConstantData(M, this);
			else if(Tm>Im){
				return new ConstantData(r.builder.CreateSExt(M, type), this);
			} else{
				return new ConstantData(r.builder.CreateTrunc(M, type), this);
			}
		} else if(V->classType==CLASS_INTLITERAL){
			const IntLiteral* IL = (const IntLiteral*)d;
			return new ConstantData(T->getValue(filePos, IL->value),this);
		} else if(V->classType==CLASS_FLOAT){
			llvm::Value* M = d->getValue(r, filePos);
			return new ConstantData(r.builder.CreateFPToSI(M, type), this);
		} else if(V->classType==CLASS_FLOATLITERAL){
			const auto& tmp = ((const FloatLiteral*)d)->value;
			const Data* ret;
			if(mpfr_nan_p(tmp)){
				ret = new ConstantData(T->getZero(filePos), this);
			} else if(mpfr_inf_p(tmp)){
				if(mpfr_signbit(tmp))
					ret = new ConstantData(T->getMinValue(), this);
				else
					ret = new ConstantData(T->getMaxValue(), this);
			}
			else{
				mpz_t out;
				mpz_init(out);
				mpfr_get_z(out, tmp, MPFR_RNDZ);
				ret = new ConstantData(T->getValue(filePos, out), this);
				mpz_clear(out);
			}
			return ret;
		}
		else{
			filePos.error("Could not find valid constructor in int");
			exit(1);
		}
	}
	case CLASS_COMPLEX:{
		if(args.size()==1){
			const Data* D = args[0]->evaluate(r);
			if(D->hasCastValue(this)){
				return D->castTo(r, this, filePos);
			}else{
				filePos.error("incomplete -- Could not find valid constructor in "+getName());
				exit(1);
			}
		}
		else if(args.size()==2){
			if(((const ComplexClass*)this)->innerClass->classType==CLASS_INT ||
					((const ComplexClass*)this)->innerClass->classType==CLASS_FLOAT){
				llvm::Value* V = llvm::UndefValue::get(this->type);
				V = r.builder.CreateInsertElement(V,
						args[0]->evaluate(r)->castToV(r, ((const ComplexClass*)this)->innerClass, filePos), getInt32(0));
				V = r.builder.CreateInsertElement(V,
								args[1]->evaluate(r)->castToV(r, ((const ComplexClass*)this)->innerClass, filePos), getInt32(1));
				return new ConstantData(V, this);
			}else{
				filePos.error("incomplete -- Could not find valid constructor in "+getName());
				exit(1);
			}
		} else{
			filePos.error("Could not find valid constructor in "+getName());
			exit(1);
		}
	}
	case CLASS_FLOAT:{
		if(args.size()==1){
			const Data* d = args[0]->evaluate(r);
			if(d->hasCastValue(this))
				return d->castTo(r, this, filePos);
		}
		filePos.compilerError("Floating and complex constructors not done yet");
		exit(1);
	}
	case CLASS_BOOL:{
		if(args.size()!=1 ) filePos.error("Could not find valid constructor in bool");
		const Data* d = args[0]->evaluate(r);
		auto V = d->getReturnType();
		if(V->classType==CLASS_STR){
			filePos.compilerError("Strings not implemented");
			exit(1);
		} else if(V->classType==CLASS_BOOL)
			return d->toValue(r, filePos);
		else{
			filePos.error("Could not find valid constructor in bool");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Lazy and Reference classes do not have constructors");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)this;
		//TODO consider alloc'ing first, then passing
		return uc->constructors.getBestFit(filePos, NO_TEMPLATE, args,false)->callFunction(r, filePos, args,nullptr);
	}
	}
}




#endif /* CONSTRUCTOR_HPP_ */
