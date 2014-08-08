/*
 * ComplexLiteralP.hpp
 *
 *  Created on: Jan 4, 2014
 *      Author: Billy
 */

#ifndef COMPLEXLITERALP_HPP_
#define COMPLEXLITERALP_HPP_

#include "ImaginaryLiteral.hpp"
#include "../../class/builtin/IntClass.hpp"
#include "../../class/builtin/FloatClass.hpp"
#include "../../class/builtin/ComplexClass.hpp"
#include "../../class/builtin/RationalClass.hpp"

const AbstractClass* ImaginaryLiteral::getReturnType() const{
	const AbstractClass* ac = imag->getReturnType();
	if(real){
		auto t = real->getReturnType();
		if(!t->hasCast(ac)){
			assert(ac->hasCast(t));
			ac = t;
		}
	}
	return ComplexClass::get((const RealClass*)ac);
}

llvm::ConstantVector* ImaginaryLiteral::getValue(RData& r, PositionID id) const{
	id.error("Cannot getValue of complex class without type");
	exit(1);
}

const Data* ImaginaryLiteral::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	if(right->classType==CLASS_VOID) return &VOID_DATA;
	if(right->classType!=CLASS_COMPLEX){
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
	const ComplexClass* cc = (const ComplexClass*)right;
	switch(cc->innerClass->classType){
	case CLASS_FLOATLITERAL:
	case CLASS_INTLITERAL:{
		auto re = real?real->castTo(r, cc->innerClass, id):nullptr;
		auto im = imag->castTo(r, cc->innerClass, id);
		if(im==imag && re==real) return this;
		else{
			if(re) assert(dynamic_cast<const Literal*>(re));
			assert(im && dynamic_cast<const Literal*>(im));
			return new ImaginaryLiteral((const Literal*)re, (const Literal*)im);
		}
	}
	case CLASS_FLOAT:
	case CLASS_INT:{
		llvm::Value* nreal = (real)?real->castToV(r, cc->innerClass, id):nullptr;
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		if(real==nullptr) ar[0] = cc->innerClass->getZero(id);
		else if(auto d = llvm::dyn_cast<llvm::Constant>(nreal)){
			ar[0] = d;
		} else ar[0] = nullptr;
		llvm::Value* nimag = imag->castToV(r, cc->innerClass, id);
		if(auto d = llvm::dyn_cast<llvm::Constant>(nimag)){
			ar[1] = d;
		} else ar[1] = nullptr;
		if(ar[0]!=nullptr && ar[1]!=nullptr)
			return new ConstantData(llvm::ConstantVector::get(ar),cc);
		else{
			llvm::Value* V = getUndef(cc->type);
			V = r.builder.CreateInsertElement(V, ar[0]?((llvm::Value*)ar[0]):nreal, getInt32(0));
			V = r.builder.CreateInsertElement(V, nimag, getInt32(1));
			return new ConstantData(V, cc);
		}
	}
	default:
		id.compilerError("Found unknown type as argument of complex");
		exit(1);
	}
}

llvm::Value* ImaginaryLiteral::castToV(RData& r, const AbstractClass* const right, const PositionID id) const{
	if(right->classType!=CLASS_COMPLEX){
		id.error("Cannot promote complex literal to non-complex type "+right->getName());
		exit(1);
	}
	const ComplexClass* cc = (const ComplexClass*)right;
	switch(cc->innerClass->classType){
	case CLASS_FLOAT:
	case CLASS_INT:{
		llvm::Value* nreal = (real)?real->castToV(r, cc->innerClass, id):nullptr;
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		if(real==nullptr) ar[0] = cc->innerClass->getZero(id);
		else if(auto d = llvm::dyn_cast<llvm::Constant>(nreal)){
			ar[0] = d;
		} else ar[0] = nullptr;
		llvm::Value* nimag = imag->castToV(r, cc->innerClass, id);
		if(auto d = llvm::dyn_cast<llvm::Constant>(nimag)){
			ar[1] = d;
		} else ar[1] = nullptr;
		if(ar[0]!=nullptr && ar[1]!=nullptr)
			return llvm::ConstantVector::get(ar);
		else{
			llvm::Value* V = getUndef(cc->type);
			V = r.builder.CreateInsertElement(V, ar[0]?((llvm::Value*)ar[0]):nreal, getInt32(0));
			V = r.builder.CreateInsertElement(V, nimag, getInt32(1));
			return V;
		}
	}
	default:
		id.compilerError("Found unknown type as argument of complex");
		exit(1);
	}
}

bool ImaginaryLiteral::hasCastValue(const AbstractClass* const a) const{
	if(a->classType==CLASS_VOID) return true;
	if(a->classType!=CLASS_COMPLEX) return false;
	const ComplexClass* cc = (const ComplexClass*)a;
	return imag->hasCastValue(cc->innerClass) && (!real || real->hasCastValue(cc->innerClass));
}

int ImaginaryLiteral::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(hasCastValue(a));
	assert(hasCastValue(b));
	if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
	else if(a->classType==CLASS_VOID) return 1;
	else if(b->classType==CLASS_VOID) return -1;
	const ComplexClass* ca = (const ComplexClass*)a;
	const ComplexClass* cb = (const ComplexClass*)a;
	auto m = imag->compareValue(ca->innerClass, cb->innerClass);
	if(!real) return m;
	auto q = real->compareValue(ca->innerClass, cb->innerClass);
	if(m<0 && q<=0) return -1;
	else if(m>0 && q>=0) return 1;
	else if(m==0 && q==0) return 0;
	else return 0;
}

#endif /* COMPLEXLITERALP_HPP_ */
