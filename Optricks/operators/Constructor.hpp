/*
 * Constructor.hpp
 *
 *  Created on: Mar 12, 2014
 *      Author: Billy
 */

#ifndef CONSTRUCTOR_HPP_
#define CONSTRUCTOR_HPP_

#include "../language/includes.hpp"

const Data* AbstractClass::callFunction(RData& r, PositionID filePos, const std::vector<const Evaluatable*>& args) const{
	switch(classType){
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:
	case CLASS_FUNC:
	case CLASS_ARRAY:
	case CLASS_VOID:
	case CLASS_CPOINTER:
	case CLASS_GEN:
	case CLASS_NULL:
	case CLASS_MAP:
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
			if(auto D = dyn_cast<ConstantInt>(M)){
				if(D->getValue().ult('0') || D->getValue().ugt('9')){
					filePos.error("Character cannot be parsed as integer");
				}
				return new ConstantData(T->getValue(filePos,(D->getLimitedValue()-'0')), this);
			}
			filePos.compilerError("char parsing not implemented");
			exit(1);
		}else if(V->classType==CLASS_STRLITERAL){
			auto M = ((const StringLiteral*)d)->value;
			//todo int parsing
			mpz_t Z;
			mpz_init_set_str(Z,M.c_str(),10);
			auto tmp = new ConstantData(T->getValue(filePos, Z), this);
			mpz_clear(Z);
			return tmp;
		}
		else if(V->classType==CLASS_INT){
			Value* M = d->getValue(r, filePos);
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
			Value* M = d->getValue(r, filePos);
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
		if(args.size()==2){
			if(((const ComplexClass*)this)->innerClass->classType==CLASS_INT ||
					((const ComplexClass*)this)->innerClass->classType==CLASS_FLOAT){
				Value* V = UndefValue::get(this->type);
				V = r.builder.CreateInsertElement(V,
						args[0]->evaluate(r)->castToV(r, ((const ComplexClass*)this)->innerClass, filePos), getInt32(0));
				V = r.builder.CreateInsertElement(V,
								args[1]->evaluate(r)->castToV(r, ((const ComplexClass*)this)->innerClass, filePos), getInt32(1));
				return new ConstantData(V, this);
			}
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
		return uc->constructors.getBestFit(filePos, args)->callFunction(r, filePos, args);
	}
	}
}




#endif /* CONSTRUCTOR_HPP_ */
