/*
 * Constructor.hpp
 *
 *  Created on: Mar 12, 2014
 *      Author: Billy
 */

#ifndef CONSTRUCTOR_HPP_
#define CONSTRUCTOR_HPP_

#include "../language/includes.hpp"
#include "../language/class/builtin/HashMapClass.hpp"

const Data* AbstractClass::callFunction(RData& r, PositionID filePos, const std::vector<const Evaluatable*>& args, const Data* instance) const{
	assert(instance==nullptr);
	switch(classType){
	case CLASS_SCOPE:
		filePos.compilerError("Scope should never be instantiated");
		exit(1);
	case CLASS_CHAR:{
		if(args.size()==1){
			const Data* d = args[0]->evaluate(r);
			if(d->type==R_STR){
				StringLiteral* s = (StringLiteral*)d;
				char c;
				if(s->value.size()!=1){
					filePos.error("Cannot convert string of length "+str(s->value.size())+" to char");
					c = '\0';
				} else c = s->value[0];
				return new ConstantData(charClass.getValue(c), &charClass);
			} else if(d->getReturnType()->classType==CLASS_CHAR){
				return d->toValue(r, filePos);
			}
		}
	}
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
	case CLASS_CSTRING:
	case CLASS_SET:
	case CLASS_CLASS:
	case CLASS_MATHLITERAL:
	case CLASS_FLOATLITERAL:
	case CLASS_INTLITERAL:
	case CLASS_RATIONAL:
	case CLASS_ENUM:
	case CLASS_WRAPPER:
	case CLASS_VECTOR:{
		filePos.error("Could not find constructor in class '"+getName()+"'");
		exit(1);
	}
	case CLASS_PRIORITYQUEUE:
	case CLASS_ARRAY:{
		llvm::Value* LEN;
		if(args.size()==0)
			LEN = getInt32(3);
		else if(args.size()>=1){
			auto L = args[0]->evaluate(r);
			auto V = L->getReturnType();
			if(V->classType==CLASS_INT){
				llvm::Value* M = L->getValue(r, filePos);
				const IntClass* I = (const IntClass*)V;
				//TODO do error check if < 0
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
		}
		const ArrayClass* tc = (const ArrayClass*)this;
		uint64_t s = llvm::DataLayout(r.lmod).getTypeAllocSize(tc->inner->type);
		llvm::Value* v;

		if(args.size()<=1){
			v = r.allocate(tc->inner->type, LEN);
		} else if(args.size()==2){
			auto M = args[1]->evaluate(r);
			if(M->hasCastValue(tc->inner)){
				v = r.allocate(tc->inner->type, LEN,M->castToV(r, tc->inner, filePos));
			} else{

				std::vector<const AbstractClass*> E_INT = {&intClass};
				std::vector<const AbstractClass*> E_LONG = {&longClass};
				const AbstractClass* V;

				if(M->hasCastValue(FunctionClass::get(tc->inner,E_INT)) ||
				   M->hasCastValue(FunctionClass::get(tc->inner,E_LONG)) ||
						( (V=M->getReturnType())->classType==CLASS_FUNC && ((FunctionClass*)V)->returnType->hasCast(tc->inner)
								&& ((FunctionClass*)V)->argumentTypes.size()==1
								&& ((FunctionClass*)V)->argumentTypes[0]->classType==CLASS_INT)){

					v = r.allocate(tc->inner->type, LEN);
					if(auto C1 = llvm::dyn_cast<llvm::ConstantInt>(r.integerCast(LEN))){
						assert(!C1->isZero());

						r.builder.CreateStore(M->callFunction(r, filePos, std::vector<const Evaluatable*>({new ConstantData(getInt32(0),&intClass)}),nullptr)->castToV(r, tc->inner, filePos), v);

						uint64_t T=C1->getValue().getLimitedValue();
						for(uint64_t i=1; i<T; i++){
							r.builder.CreateStore(M->callFunction(r, filePos, std::vector<const Evaluatable*>({new ConstantData(getInt32(i),&intClass)}),nullptr)->castToV(r, tc->inner, filePos), r.builder.CreateConstGEP1_64(v, i));
						}
					} else {
						auto START= r.builder.GetInsertBlock();
						auto FUNC = START->getParent();
						auto LOOP = r.CreateBlockD("loop", FUNC);
						auto DONE = r.CreateBlockD("done", FUNC);
						auto CZ = llvm::ConstantInt::get(INT32TYPE,0,false);
						auto SIZE = r.integerCast(LEN, INT32TYPE);
						r.builder.CreateCondBr(r.builder.CreateICmpSLE(SIZE, CZ), DONE, LOOP);
						r.builder.SetInsertPoint(LOOP);
						auto idx = r.builder.CreatePHI(SIZE->getType(),2);
						idx->addIncoming(CZ, START);
						auto P1 = r.builder.CreateAdd(idx, llvm::ConstantInt::get(SIZE->getType(),1,false));
						idx->addIncoming(P1, LOOP);
						r.builder.CreateStore(M->callFunction(r, filePos, std::vector<const Evaluatable*>({new ConstantData(idx,&intClass)}),nullptr)->castToV(r, tc->inner, filePos), r.builder.CreateGEP(v, idx));
						r.builder.CreateCondBr(r.builder.CreateICmpEQ(SIZE, P1), DONE, LOOP);
						r.builder.SetInsertPoint(DONE);
					}
				} else filePos.error("Could not find valid constructor in array");

			}
		} else filePos.error("Could not find valid constructor in array");

		assert(llvm::dyn_cast<llvm::PointerType>(tc->type));
		auto tmp=(llvm::StructType*)(((llvm::PointerType*)tc->type)->getElementType());

		auto p = r.allocate(tmp);


		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));
		r.builder.CreateStore((args.size()==2)?LEN:(llvm::Value*)getInt32(0),
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(LEN,
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		r.builder.CreateStore(v,G);
		return new ConstantData(p, this);
	}
	case CLASS_HASHMAP:{
		if(args.size()>1 ) filePos.error("Could not find valid constructor in array");
		llvm::Value* LEN;
		if(args.size()==0){
			LEN = getInt32(3);
		} else {
			auto L = args[0]->evaluate(r);
			auto V = L->getReturnType();
			if(V->classType==CLASS_INT){
				llvm::Value* M = L->getValue(r, filePos);
				const IntClass* I = (const IntClass*)V;
				auto Im = I->getWidth();
				LEN = r.builder.CreateSExtOrTrunc(M, intClass.type);
			} else if(V->classType==CLASS_INTLITERAL){
				const IntLiteral* IL = (const IntLiteral*)L;
				assert(intClass.getWidth()==32);
				LEN = intClass.getValue(filePos, IL->value);
			}
		}
		const HashMapClass* tc = (const HashMapClass*)this;
		auto PT = llvm::PointerType::getUnqual(tc->nodeType);

		auto v = r.allocate(PT, LEN, llvm::ConstantPointerNull::get(PT));

		assert(llvm::dyn_cast<llvm::PointerType>(tc->type));
		auto tmp=(llvm::StructType*)(((llvm::PointerType*)tc->type)->getElementType());

		auto p = r.allocate(tmp);

		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));

		r.builder.CreateStore(getInt32(0),
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(LEN,
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		r.builder.CreateStore(v,G);
		return new ConstantData(p, this);
	}
	case CLASS_INT:{
		const Data* d;
		if(args.size()==2){
			d = args[0]->evaluate(r);
			const Data* d2 = args[1]->evaluate(r);
			auto V = d->getReturnType();
			if(d2->getReturnType()->classType==CLASS_BOOL && V->classType==CLASS_INT){
				return new ConstantData(r.builder.CreateZExtOrTrunc(d->getValue(r, filePos),type), this);
			}
		} else if(args.size()==1) d = args[0]->evaluate(r);
		if(args.size()!=1 ) filePos.error("Could not find valid constructor in bool");
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
			return &VOID_DATA;
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
	case CLASS_LAZY:{
		filePos.error("Lazy classes do not have constructors");
		exit(1);
	}
	case CLASS_REF:{
		//const Data* d;
		//if(args.size()!=1  || (d=args[0]->evaluate(r))->getReturnType()->classType!=CLASS_CPOINTER){
			filePos.error("Reference classes do not have constructors");
			return &VOID_DATA;
		//}
		//return new LocationData(new StandardLocation(r.builder.CreatePointerCast(d->getValue(r, filePos),type)),((const ReferenceClass*)this)->innerType);
		//filePos.error("Lazy classes do not have constructors");
		//exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)this;
		//TODO consider alloc'ing first, then passing
		return uc->constructors.getBestFit(filePos, args,false)->callFunction(r, filePos, args,nullptr);
	}
	}
}




#endif /* CONSTRUCTOR_HPP_ */
