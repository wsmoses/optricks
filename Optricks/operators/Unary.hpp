/*
 * Unary.hpp
 *
 *  Created on: Mar 5, 2014
 *      Author: Billy
 */

#ifndef UNARY_HPP_
#define UNARY_HPP_
#include "../language/class/AbstractClass.hpp"
#include "../language/class/builtin/VectorClass.hpp"
inline const AbstractClass* getPreopReturnType(PositionID filePos, const AbstractClass* cc, const String operation){
	if(operation=="&"){
		if(cc->classType==CLASS_REF) filePos.error("Cannot get reference of reference");
		if(cc->classType==CLASS_LAZY) filePos.error("Cannot get reference of lazy");
		else return ReferenceClass::get(cc);
	}
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		return VectorClass::get(getPreopReturnType(filePos, vc->inner, operation), vc->len);
	}
	case CLASS_INT:{
		if(operation=="+") return cc;
		else if(operation=="-") return cc;
		else if(operation=="~") return cc;
		else if(operation=="++") return cc;
		else if(operation=="--") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_INTLITERAL:{
		const IntLiteralClass* ilc = (const IntLiteralClass*)cc;
		if(operation=="+") return cc;
		else if(operation=="-"){
			mpz_t v;
			mpz_init(v);
			mpz_neg(v, ilc->value);
			return IntLiteralClass::get(v);
		}
		else if(operation=="~"){
			mpz_t v;
			mpz_init(v);
			mpz_add_ui(v, ilc->value, 1);
			mpz_neg(v, v);//todo check this
			return IntLiteralClass::get(v);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOATLITERAL:{
		if(operation=="+") return cc;
		else if(operation=="-") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_RATIONAL:
	case CLASS_COMPLEX:
	case CLASS_FLOAT:{
		if(operation=="+") return cc;
		else if(operation=="-") return cc;
		else if(operation=="++") return cc;
		else if(operation=="--") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_BOOL:{
		if(operation=="!") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
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
	case CLASS_AUTO:
	case CLASS_SET:
	case CLASS_CLASS:
	case CLASS_MATHLITERAL:{
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply unary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getPreop(filePos, operation).returnType;
	}
	}
}

inline const Data* getPreop(RData& r, PositionID filePos, const String operation, const Data* value){
	const AbstractClass* const cc = value->getReturnType();
	if(operation=="&"){
		if(cc->classType==CLASS_REF) filePos.error("Cannot get reference of reference");
		if(cc->classType==CLASS_LAZY) filePos.error("Cannot get reference of lazy");
		else{
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for reference");
			const LocationData* ld = (const LocationData*)value;
			return new ConstantData(ld->value->getPointer(r, filePos), ReferenceClass::get(cc));
		}
	}
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		return VectorClass::get(getPreopReturnType(filePos, vc->inner, operation), vc->len);
	}
	case CLASS_INT:{
		if(operation=="+") return value->toValue(r, filePos);
		else if(operation=="-"){
			return new ConstantData(r.builder.CreateNeg(value->getValue(r, filePos)), cc);
		}
		else if(operation=="~"){
			return new ConstantData(r.builder.CreateNot(value->getValue(r, filePos)), cc);
		}
		else if(operation=="++"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateAdd(toR, ic->getOne(filePos)), r);
			return new ConstantData(toR, cc);
		}
		else if(operation=="--"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateSub(toR, ic->getOne(filePos)), r);
			return new ConstantData(toR, cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_INTLITERAL:{
		const IntLiteralClass* ilc = (const IntLiteralClass*)cc;
		if(operation=="+") return value;
		else if(operation=="-"){
			mpz_t v;
			mpz_init(v);
			mpz_neg(v, ilc->value);
			auto R = new IntLiteral(IntLiteralClass::get(v));
			mpz_clear(v);
			return R;
		}
		else if(operation=="~"){
			mpz_t v;
			mpz_init(v);
			mpz_add_ui(v, ilc->value, 1);
			mpz_neg(v, v);//todo check this
			auto R = new IntLiteral(IntLiteralClass::get(v));
			mpz_clear(v);
			return R;
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOATLITERAL:{
		if(operation=="+") return value;
		else if(operation=="-"){
			const FloatLiteral* fl = (const FloatLiteral*)value;
			mpfr_t v;
			mpfr_init(v);
			mpfr_neg(v, fl->value, MPFR_RNDN);
			auto R = new FloatLiteral(v);
			mpfr_clear(v);
			return R;
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_RATIONAL:{
		//todo
		filePos.compilerError("Must finish implementing rational class");
		exit(1);
	}
	case CLASS_COMPLEX:{
		if(operation=="+") return value->toValue(r,filePos);
		else if(operation=="-"){
			const ComplexClass* CC = (const ComplexClass*)cc;
			if(CC->innerClass->classType==CLASS_INT)
				return new ConstantData(r.builder.CreateNeg(value->getValue(r, filePos)), cc);
			else if(CC->innerClass->classType==CLASS_FLOAT)
				return new ConstantData(r.builder.CreateFNeg(value->getValue(r, filePos)), cc);
			else{
				assert(CC->innerClass->classType==CLASS_RATIONAL);
				filePos.compilerError("Need to implement rationals");
			}
		}
		else if(operation=="++"){
			//todo setstruct for location info
			filePos.compilerError("Complex preops not implemented ++ ");
			exit(1);
		}
		else if(operation=="--"){
			//todo setstruct for location info
			filePos.compilerError("Complex preops not implemented ++ ");
			exit(1);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOAT:{
		if(operation=="+") return value->toValue(r, filePos);
		else if(operation=="-"){
			return new ConstantData(r.builder.CreateFNeg(value->getValue(r, filePos)), cc);
		}
		else if(operation=="++"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateFAdd(toR, ic->getOne(filePos)), r);
			return new ConstantData(toR, cc);
		}
		else if(operation=="--"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateFSub(toR, ic->getOne(filePos)), r);
			return new ConstantData(toR, cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_BOOL:{
		if(operation=="!"){
			return new ConstantData(r.builder.CreateNot(value->getValue(r, filePos)), cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
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
	case CLASS_AUTO:
	case CLASS_SET:
	case CLASS_CLASS:
	case CLASS_MATHLITERAL:{
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply unary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getPreop(filePos, operation).returnType;
	}
	}
}
inline const AbstractClass* getPostopReturnType(PositionID filePos, const AbstractClass* cc, const String operation){
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		return VectorClass::get(getPostopReturnType(filePos, vc->inner, operation), vc->len);
	}
	case CLASS_COMPLEX:
	case CLASS_FLOAT:
	case CLASS_RATIONAL:
	case CLASS_INT:{
		if(operation=="++") return cc;
		else if(operation=="--") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_BOOL:
	case CLASS_INTLITERAL:
	case CLASS_FLOATLITERAL:
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:
	case CLASS_FUNC:
	case CLASS_ARRAY:
	case CLASS_VOID:
	case CLASS_CPOINTER:
	case CLASS_GEN:
	case CLASS_NULL:
	case CLASS_MAP:
	case CLASS_MATHLITERAL:
	case CLASS_STR:
	case CLASS_CHAR:
	case CLASS_AUTO:
	case CLASS_SET:
	case CLASS_CLASS:{
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply unary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getPreop(filePos, operation).returnType;
	}
	}
}

inline const Data* getPostop(RData& r, PositionID filePos, const String operation, const Data* value){
	const AbstractClass* const cc = value->getReturnType();
	switch(cc->classType){
	case CLASS_VECTOR:{
		filePos.compilerError("Vector postops not implemented");
		exit(1);
		//const VectorClass* vc = (const VectorClass*)cc;
		//VectorClass::get(getPostopReturnType(filePos, vc->inner, operation), vc->len);
		//return fdafsda;
	}
	case CLASS_INT:{
		if(operation=="++"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateAdd(toR, ic->getOne(filePos));
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else if(operation=="--"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateSub(toR, ic->getOne(filePos));
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_RATIONAL:{
		//todo
		filePos.compilerError("Must finish implementing rational class");
		exit(1);
	}
	case CLASS_COMPLEX:{
		//todo
		filePos.compilerError("Complex postops not implemented");
		exit(1);
		if(operation=="++"){
			//todo setstruct for location info
		}
		else if(operation=="--"){
			//todo setstruct for location info
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOAT:{
		if(operation=="++"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateFAdd(toR, ic->getOne(filePos));
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else if(operation=="--"){
			const RealClass* ic = (const RealClass*)cc;
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+ic->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateFSub(toR, ic->getOne(filePos));
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_BOOL:
	case CLASS_INTLITERAL:
	case CLASS_FLOATLITERAL:
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
	case CLASS_AUTO:
	case CLASS_SET:
	case CLASS_CLASS:
	case CLASS_MATHLITERAL:{
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply unary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getPreop(filePos, operation).returnType;
	}
	}
}
#endif /* UNARY_HPP_ */
