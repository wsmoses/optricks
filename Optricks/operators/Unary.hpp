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
#include "../language/data/literal/MathConstantLiteral.hpp"
#include "../language/data/ReferenceData.hpp"
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
		if(operation==":str") return &stringLiteralClass;
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

		if(operation==":str"){
			return &stringLiteralClass;
		}
		else if(operation=="+" || operation=="-" || operation=="~") return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOATLITERAL:{

		if(operation==":str"){
			return &stringLiteralClass;
		}
		else if(operation=="+") return cc;
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
	case CLASS_MATHLITERAL:{
		const MathConstantClass* mlc = (const MathConstantClass*)cc;
		if(operation==":str"){
			return &stringLiteralClass;
		}
		else if(operation=="+") return mlc;
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STRLITERAL:{
		if(operation==":str"){
			return &stringLiteralClass;
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CHAR:{
		if(operation==":str"){
			return &stringLiteralClass;
		}
		else if(operation=="++") return cc;
		else if(operation=="--") return cc;
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

inline const Data* getPreop(RData& r, PositionID filePos, const String operation, const Data* value){
	const AbstractClass* const cc = value->getReturnType();
	if(operation=="&"){
		if(cc->classType==CLASS_REF) filePos.error("Cannot get reference of reference");
		if(cc->classType==CLASS_LAZY) filePos.error("Cannot get reference of lazy");
		else{
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for reference");
			const LocationData* ld = (const LocationData*)value;
			return new ReferenceData(ld);
		}
	}
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		return VectorClass::get(getPreopReturnType(filePos, vc->inner, operation), vc->len);
	}
	case CLASS_INT:{
		if(operation==":str"){
			Value* V = value->getValue(r, filePos);
			if(auto C = dyn_cast<ConstantInt>(V)){
				return new StringLiteral(C->getValue().toString(10,true));
			}
		}
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
		const IntLiteral* il = (const IntLiteral*)value;
		if(operation==":str"){
			return new StringLiteral(il->toString());
		}
		else if(operation=="+") return value;
		else if(operation=="-"){
			auto R = new IntLiteral(0,0,0);
			mpz_neg(R->value, il->value);
			return R;
		}
		else if(operation=="~"){
			auto R = new IntLiteral(0,0,0);
			mpz_add_ui(R->value, il->value, 1);
			mpz_neg(R->value, R->value);//todo check this
			return R;
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_FLOATLITERAL:{
		if(operation==":str"){
			return new StringLiteral(((const FloatLiteral*)value)->toString());
		}
		else if(operation=="+") return value;
		else if(operation=="-"){
			const FloatLiteral* fl = (const FloatLiteral*)value;
			auto R = new FloatLiteral(0,0,0);
			mpfr_neg(R->value, fl->value, MPFR_RNDN);
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
		break;
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
	case CLASS_MATHLITERAL:{
		const MathConstantClass* mlc = (const MathConstantClass*)cc;
		if(operation==":str"){
			return new StringLiteral(((const MathConstantLiteral*)value)->toString());
		}
		else if(operation=="+") return value;
		if(false) return cc;
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STRLITERAL:{
		if(operation==":str"){
			return value;
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CHAR:{
		if(operation==":str"){
			if(auto CC = dyn_cast<ConstantInt>(value->getValue(r, filePos))){
				return new StringLiteral(String(1,(char) CC->getLimitedValue()));
			}
		}
		else if(operation=="++"){
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+cc->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateAdd(toR, charClass.getOne()), r);
			return new ConstantData(toR, &charClass);
		}
		else if(operation=="--"){
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for pre operator "+operation+" in class '"+cc->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			L->setValue(r.builder.CreateSub(toR, charClass.getOne()), r);
			return new ConstantData(toR, &charClass);
		}
		filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
		exit(1);
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
inline const AbstractClass* getPostopReturnType(PositionID filePos, const AbstractClass* cc, const String operation){
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		return VectorClass::get(getPostopReturnType(filePos, vc->inner, operation), vc->len);
	}
	case CLASS_COMPLEX:
	case CLASS_FLOAT:
	case CLASS_RATIONAL:
	case CLASS_CHAR:
	case CLASS_INT:{
		if(operation=="++") return cc;
		else if(operation=="--") return cc;
		else{
			filePos.error("Could not find unary ost-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CLASS:{
		if(operation=="&") return &classClass;
		if(operation=="[]") return &classClass;
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
	case CLASS_SET:{
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
		break;
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
	case CLASS_CHAR:{
		if(operation=="++"){
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for post operator "+operation+" in class '"+cc->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateAdd(toR, charClass.getOne());
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else if(operation=="--"){
			if(value->type!=R_LOC) filePos.error("Cannot use non-variable for post operator "+operation+" in class '"+cc->getName()+"'");
			auto L = ((const LocationData*)value)->value;
			Value* toR = L->getValue(r, filePos);
			Value* toS = r.builder.CreateSub(toR, charClass.getOne());
			L->setValue(toS, r);
			return new ConstantData(toS, cc);
		}
		else{
			filePos.error("Could not find unary pre-operation '"+operation+"' in class '"+cc->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CLASS:{
		if(operation=="&"){
			return ReferenceClass::get(value->getMyClass(r, filePos));
		} else if(operation=="[]"){
			return ArrayClass::get(value->getMyClass(r, filePos),0);
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
	case CLASS_SET:
	case CLASS_MATHLITERAL:{
		if(false) return cc;
		else{
			filePos.error("Could not find unary post-operation '"+operation+"' in class '"+cc->getName()+"'");
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
