/*
 * Binary.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: Billy
 */

#ifndef BINARY_HPP_
#define BINARY_HPP_

#include "../language/class/AbstractClass.hpp"
#include "../language/class/builtin/VectorClass.hpp"
#include "../language/evaluatable/CastEval.hpp"
inline const AbstractClass* getBinopReturnType(PositionID filePos, const AbstractClass* cc, const AbstractClass* dd, const String operation){
	if(operation=="+"){
		if(cc->classType==CLASS_STR) return cc;
		if(dd->classType==CLASS_STR) return dd;
	}
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		if(dd->classType!=CLASS_VECTOR){
			if(operation!="==" && operation!="!="){
			}
			filePos.error("Cannot have binary operation between vector and non-vector type");
		}
		if(operation=="==" || operation=="!=") return boolClass;
		filePos.compilerError("Vector binops not implemented");
		exit(1);
		//return VectorClass::get(getBinopReturnType(filePos, vc->inner, operation), vc->classType);
	}
	case CLASS_INT:{
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* max = (const IntClass*)cc;
			if(max->getWidth()<=((const IntClass*)dd)->getWidth()){
				max = (const IntClass*)dd;
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|"
							|| operation=="<<" || operation==">>" || operation==">>>") return max;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|"
							|| operation=="<<" || operation==">>" || operation==">>>") return cc;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOAT:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return dd;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(!cc->hasCast(comp->innerClass)) {
				if(!comp->innerClass->hasCast(cc)){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
				else comp = ComplexClass::get((const RealClass*)cc);
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return dd;
			else if(operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOATLITERAL:{
			filePos.error("Floating literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		case CLASS_MATHLITERAL:{
			filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_INTLITERAL:{
		const IntLiteralClass* ilc = (const IntLiteralClass*)cc;
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* R = (const IntClass*)dd;
			if(!R->hasFit(ilc->value)) filePos.error("Cannot fit integer literal "+ilc->getName()+" in integer type "+R->getName());
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|"
							|| operation=="<<" || operation==">>" || operation==">>>") return R;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|"
							|| operation=="<<" || operation==">>" || operation==">>>"){
				//todo
				filePos.compilerError("Must have correct integer literal calculation");
				exit(1);
			}
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOAT:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return dd;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return dd;
			else if(operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOATLITERAL:{
			filePos.error("Floating literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		case CLASS_MATHLITERAL:{
			if(operation=="==" || operation=="!=" || operation==">=" || operation=="<=") return boolClass;
			else if(operation=="*" && cc==IntLiteralClass::get(0)){
				return cc;
			} else if(operation=="*" && cc==IntLiteralClass::get(1)){
				return dd;
			} else if(operation=="+" && cc==IntLiteralClass::get(0)){
				return dd;
			} else {
				filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOAT:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			const FloatClass* max = (const FloatClass*)cc;
			if(max->getWidth()<=((const FloatClass*)dd)->getWidth()){
				max = (const FloatClass*)dd;
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return max;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_MATHLITERAL:
		case CLASS_INTLITERAL:
		case CLASS_FLOATLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return cc;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(!cc->hasCast(comp->innerClass)) {
				if(!comp->innerClass->hasCast(cc)){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
				else comp = ComplexClass::get((const RealClass*)cc);
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return comp;
			else if(operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOATLITERAL:{
		switch(dd->classType){
		case CLASS_FLOATLITERAL:
		case CLASS_FLOAT:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return dd;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return cc;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(!cc->hasCast(comp->innerClass)) {
				if(!comp->innerClass->hasCast(cc)){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
				else comp = ComplexClass::get((const RealClass*)cc);
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return comp;
			else if(operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_MATHLITERAL:
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_MATHLITERAL:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" ) return dd;
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_MATHLITERAL:{
			if(dd==cc){
				if(operation=="-") return IntLiteralClass::get(0);
				else if(operation=="/") return IntLiteralClass::get(1);
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(comp->innerClass->classType!=CLASS_FLOAT){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**") return comp;
			else if(operation=="==" || operation=="!=") return boolClass;
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_COMPLEX:{
		const ComplexClass* comp = (const ComplexClass*)cc;
		switch(comp->innerClass->classType){
		case CLASS_FLOATLITERAL:
		case CLASS_INTLITERAL:{
			filePos.compilerError("todo -- complex binops 1");
			exit(1);
		}
		case CLASS_INT:
		case CLASS_FLOAT:{
			filePos.compilerError("todo -- complex binops 2");
			exit(1);
		}
		default:{
			filePos.compilerError("complex - this should never happen!");
			exit(1);
		}
		}
		break;
	}
	case CLASS_BOOL:{
		if(operation=="||" || operation=="&&" || operation=="|" || operation=="&"
				|| operation=="==" || operation=="!=") return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STR:
	case CLASS_CHAR:{
		if(dd->classType==CLASS_STR || dd->classType==CLASS_CHAR) return boolClass;
		filePos.compilerError("todo -- string binops");
		exit(1);
	}
	case CLASS_CPOINTER:
	case CLASS_NULL:{
		if((dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->classType==CLASS_NULL || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT)
			&& (operation=="==" || operation=="!=")) return boolClass;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_RATIONAL:{
		filePos.compilerError("Todo -- implement rationals");
		exit(1);
	}
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:{
		const TupleClass* t1 = (const TupleClass*)cc;
		const TupleClass* t2 = (const TupleClass*)dd;
		if(t1->innerTypes.size()!=t2->innerTypes.size() || (operation!="==" && operation!="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		for(unsigned i=0; i<t1->innerTypes.size(); i++){
			if(getBinopReturnType(filePos, t1->innerTypes[i], t2->innerTypes[i], operation)->classType!=CLASS_BOOL){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		if(t1->classType==CLASS_NAMED_TUPLE && t2->classType==CLASS_NAMED_TUPLE){
			const NamedTupleClass* nt1 = (const NamedTupleClass*)cc;
			const NamedTupleClass* nt2 = (const NamedTupleClass*)dd;

			for(unsigned i=0; i<t1->innerTypes.size(); i++){
				if(nt1->innerNames[i]!=nt2->innerNames[i]){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
			}
		}
		return boolClass;
	}
	case CLASS_CLASS:
	case CLASS_FUNC:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		return boolClass;
	}
	case CLASS_ARRAY:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==CLASS_ARRAY || dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		const ArrayClass* a1 = (const ArrayClass*)cc;
		const ArrayClass* a2 = (const ArrayClass*)dd;
		if(a1->len!=a2->len || a1->inner!=a2->inner){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		return boolClass;
	}
	case CLASS_GEN:
	case CLASS_SET:
	case CLASS_MAP:
	case CLASS_AUTO:
	case CLASS_VOID:{
		if(false) return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply binary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getLocalFunction(filePos, ":"+operation, std::vector<const AbstractClass*>({cc, dd}))->getSingleProto()->returnType;
	}
	}
}

inline const Data* getBinop(RData& r, PositionID filePos, const Data* value, const Evaluatable* ev, const String operation){
	const AbstractClass* cc = value->getReturnType();
	const AbstractClass* dd = ev->getReturnType();
	if(operation=="+"){
		if(cc->classType==CLASS_STR){
			filePos.error("Todo -- string binop 1");
		}
		if(dd->classType==CLASS_STR){
			filePos.error("Todo -- string binop 2");
		}
	}
	switch(cc->classType){
	case CLASS_VECTOR:{
		const VectorClass* vc = (const VectorClass*)cc;
		if(dd->classType!=CLASS_VECTOR){
			if(operation!="==" && operation!="!="){
			}
			filePos.error("Cannot have binary operation between vector and non-vector type");
		}
		//if(operation=="==" || operation=="!=") return boolClass;
		filePos.compilerError("Vector binops not implemented");
		exit(1);
		//return VectorClass::get(getBinopReturnType(filePos, vc->inner, operation), vc->classType);
	}
	case CLASS_INT:{
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* max = (const IntClass*)cc;
			if(max->getWidth()<=((const IntClass*)dd)->getWidth()){
				max = (const IntClass*)dd;
			}
			if(operation=="+") return new ConstantData(r.builder.CreateAdd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="-") return new ConstantData(r.builder.CreateSub(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="*") return new ConstantData(r.builder.CreateMul(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="/") return new ConstantData(r.builder.CreateSDiv(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="%") return new ConstantData(r.builder.CreateSRem(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="&") return new ConstantData(r.builder.CreateAnd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="|")	return new ConstantData(r.builder.CreateOr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="<<") return new ConstantData(r.builder.CreateShl(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">>") return new ConstantData(r.builder.CreateAShr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">>>") return new ConstantData(r.builder.CreateLShr(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">") return new ConstantData(r.builder.CreateICmpSGT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation==">=") return new ConstantData(r.builder.CreateICmpSGE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="<") return new ConstantData(r.builder.CreateICmpSLT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="<=") return new ConstantData(r.builder.CreateICmpSLE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="!=") return new ConstantData(r.builder.CreateICmpNE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="**"){
				filePos.compilerError("Todo -- integer pow");
				exit(1);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INTLITERAL:{
			return getBinop(r, filePos, value, ev->evaluate(r)->castTo(r, cc, filePos), operation);
		}
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(cc->hasCast(comp->innerClass)) {
				return getBinop(r, filePos, value->castTo(r, comp, filePos), ev, operation);
			}
			if(comp->innerClass->hasCast(cc)){
				comp = ComplexClass::get((const RealClass*)cc);
				return getBinop(r, filePos, value->castTo(r, comp, filePos), new CastEval(ev, comp, filePos), operation);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOATLITERAL:{
			filePos.error("Floating literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		case CLASS_MATHLITERAL:{
			filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_INTLITERAL:{
		const IntLiteralClass* ilc = (const IntLiteralClass*)cc;
		switch(dd->classType){
		case CLASS_INT:{
			const IntClass* R = (const IntClass*)dd;
			if(!R->hasFit(ilc->value)) filePos.error("Cannot fit integer literal "+ilc->getName()+" in integer type "+R->getName());
			return getBinop(r, filePos, value->castTo(r, R, filePos), ev, operation);
		}
		case CLASS_INTLITERAL:{
			if(operation=="+" || operation=="-" || operation=="*" || operation=="/" || operation=="%"
					|| operation=="**" || operation=="&" || operation=="|"
							|| operation=="<<" || operation==">>" || operation==">>>"){
				//todo
				filePos.compilerError("Must have correct integer literal calculation");
				exit(1);
			}
			else if(operation==">" || operation==">=" || operation=="<" || operation=="<="
					|| operation=="==" || operation=="!="){
				//todo
				filePos.compilerError("Intliteral comp not complete");
				exit(1);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_FLOAT:
		case CLASS_COMPLEX:{
			if(((const ComplexClass*)dd)->innerClass->classType==CLASS_INTLITERAL){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_FLOATLITERAL:{
			filePos.error("Floating literal class cannot combine with integer types directly -- cast to floating-point type first");
			exit(1);
		}
		case CLASS_MATHLITERAL:{
			if(operation=="==" || operation=="!=" || operation==">=" || operation=="<=") return boolClass;
			else if(operation=="*" && cc==IntLiteralClass::get(0)){
				return value;
			} else if(operation=="*" && cc==IntLiteralClass::get(1)){
				return ev->evaluate(r);
			} else if(operation=="+" && cc==IntLiteralClass::get(0)){
				return ev->evaluate(r);
			} else {
				filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOAT:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			const FloatClass* max = (const FloatClass*)cc;
			if(max->getWidth()<=((const FloatClass*)dd)->getWidth()){
				max = (const FloatClass*)dd;
			}
			if(operation=="+") return new ConstantData(r.builder.CreateFAdd(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="-") return new ConstantData(r.builder.CreateFSub(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="*") return new ConstantData(r.builder.CreateFMul(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="/") return new ConstantData(r.builder.CreateFDiv(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation=="%") return new ConstantData(r.builder.CreateFRem(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			else if(operation==">") return new ConstantData(r.builder.CreateFCmpOGT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation==">=") return new ConstantData(r.builder.CreateFCmpOGE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="<") return new ConstantData(r.builder.CreateFCmpOLT(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="<=") return new ConstantData(r.builder.CreateFCmpOLE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="==") return new ConstantData(r.builder.CreateFCmpOEQ(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="!=") return new ConstantData(r.builder.CreateFCmpONE(value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), boolClass);
			else if(operation=="**"){
				auto IN = llvm::Intrinsic::getDeclaration(& r.lmod, llvm::Intrinsic::pow, SmallVector<Type*,1>(1,max->type));
				return new ConstantData(r.builder.CreateCall2(IN, value->castToV(r, max, filePos), ev->evaluate(r)->castToV(r, max, filePos)), max);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		case CLASS_INT:
		case CLASS_INTLITERAL:{
			if(operation=="**" && dd->hasCast(intClass)){
				auto IN = llvm::Intrinsic::getDeclaration(& r.lmod, llvm::Intrinsic::pow, SmallVector<Type*,1>(1,cc->type));
				return new ConstantData(r.builder.CreateCall2(IN, value->getValue(r, filePos), ev->evaluate(r)->castToV(r, intClass, filePos)), cc);
			} else return getBinop(r, filePos, value, new CastEval(ev, cc, filePos), operation);
		}
		case CLASS_MATHLITERAL:
		case CLASS_FLOATLITERAL:{
			return getBinop(r, filePos, value, new CastEval(ev, cc, filePos), operation);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(cc->hasCast(comp->innerClass)) {
				return getBinop(r, filePos, value->castTo(r, comp, filePos), ev, operation);
			}
			if(comp->innerClass->hasCast(cc)){
				comp = ComplexClass::get((const RealClass*)cc);
				return getBinop(r, filePos, value->castTo(r, comp, filePos), new CastEval(ev, comp, filePos), operation);
			}
			else {
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_FLOATLITERAL:{
		switch(dd->classType){
		case CLASS_FLOATLITERAL:{
			assert(value->type==R_FLOAT);
			auto tmp = ev->evaluate(r);
			assert(tmp->type==R_FLOAT);
			mpfr_t M;
			mpfr_init_set(M, ((const FloatLiteral*)value)->value, MPFR_RNDN);
			mpfr_add(M, M, ((const FloatLiteral*)tmp)->value, MPFR_RNDN);
			return new FloatLiteral(M);
		}
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_INTLITERAL:{
			return getBinop(r, filePos, value, new CastEval(ev, cc, filePos), operation);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			switch(comp->innerClass->classType){
			case CLASS_FLOATLITERAL:
			case CLASS_INTLITERAL:{
				filePos.compilerError("literal complex not defined");
				exit(1);
			}
			case CLASS_INT:
			case CLASS_FLOAT:{
				return getBinop(r, filePos, value->castTo(r, comp, filePos), ev, operation);
			}
			default:{
				filePos.compilerError("Strange condition for complex");
				exit(1);
			}
			}
		}
		case CLASS_INT:
		case CLASS_MATHLITERAL:
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_MATHLITERAL:{
		switch(dd->classType){
		case CLASS_FLOAT:{
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		case CLASS_MATHLITERAL:{
			if(dd==cc){
				if(operation=="-") return new IntLiteral(IntLiteralClass::get(0));
				else if(operation=="/") return new IntLiteral(IntLiteralClass::get(1));
			}
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* comp = (const ComplexClass*)dd;
			if(comp->innerClass->classType!=CLASS_FLOAT){
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
			return getBinop(r, filePos, value->castTo(r, dd, filePos), ev, operation);
		}
		default:{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		}
		break;
	}
	case CLASS_COMPLEX:{
		const ComplexClass* comp = (const ComplexClass*)cc;
		switch(comp->innerClass->classType){
		case CLASS_FLOATLITERAL:
		case CLASS_INTLITERAL:{
			filePos.compilerError("todo -- complex binops 1");
			exit(1);
		}
		case CLASS_INT:
		case CLASS_FLOAT:{//todo
			filePos.compilerError("todo -- complex binops 2");
			exit(1);
		}
		default:{
			filePos.compilerError("complex - this should never happen!");
			exit(1);
		}
		}
		break;
	}
	case CLASS_BOOL:{
		if(dd->classType!=CLASS_BOOL){
			filePos.compilerError("complex - this should never happen!");
			exit(1);
		}
		if(operation=="&") return new ConstantData(r.builder.CreateAnd(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="|") return new ConstantData(r.builder.CreateOr(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="!=") return new ConstantData(r.builder.CreateICmpNE(value->getValue(r, filePos), ev->evaluate(r)->getValue(r, filePos)), cc);
		else if(operation=="||"){
			Value* Start = value->castToV(r, boolClass, filePos);

			BasicBlock* StartBB = r.builder.GetInsertBlock();
			BasicBlock *ElseBB;
			BasicBlock *MergeBB;
			if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
				if(!c->isOne()) return ev->evaluate(r)->castTo(r,boolClass,filePos);
				else return new BoolLiteral(true);
			}
			else{
				ElseBB = r.CreateBlock("else",StartBB);
				MergeBB = r.CreateBlock("ifcont",StartBB);
				r.builder.CreateCondBr(Start, MergeBB, ElseBB);
			}
			StartBB = r.builder.GetInsertBlock();
			r.builder.SetInsertPoint(ElseBB);
			Value* fin = ev->evaluate(r)->castToV(r,boolClass,filePos);
			//TODO can allow check if right is constant
			r.builder.CreateBr(MergeBB);
			ElseBB = r.builder.GetInsertBlock();
			//a.addPred(MergeBB,ElseBB);
			r.builder.SetInsertPoint(MergeBB);
			PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
			PN->addIncoming(Start, StartBB);
			PN->addIncoming(fin, ElseBB);
			return new ConstantData(PN, boolClass);
		}
		else if(operation=="&&"){
			Value* Start = value->castToV(r, boolClass, filePos);
			BasicBlock* StartBB = r.builder.GetInsertBlock();
			BasicBlock *ElseBB;
			BasicBlock *MergeBB;
			if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
				if(c->isOne()) return ev->evaluate(r)->castTo(r,boolClass,filePos);
				else return new BoolLiteral(false);
			}
			else{
				ElseBB = r.CreateBlock("else",StartBB);
				MergeBB = r.CreateBlock("ifcont",StartBB);
				r.builder.CreateCondBr(Start, ElseBB, MergeBB);
			}
			r.builder.SetInsertPoint(ElseBB);
			Value* fin = ev->evaluate(r)->castToV(r,boolClass,filePos);
			//TODO can allow check if right is constant
			r.builder.CreateBr(MergeBB);
			ElseBB = r.builder.GetInsertBlock();
			//a.addPred(MergeBB,ElseBB);
			r.builder.SetInsertPoint(MergeBB);
			PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
			PN->addIncoming(Start, StartBB);
			PN->addIncoming(fin, ElseBB);
			return new ConstantData(PN, boolClass);
		}
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_STR:
	case CLASS_CHAR:{
		if(dd->classType==CLASS_STR || dd->classType==CLASS_CHAR) return boolClass;
		filePos.compilerError("todo -- string binops");
		exit(1);
	}
	case CLASS_NULL:{
		if((dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->classType==CLASS_NULL || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT)
			&& (operation=="==" || operation=="!=")){
			auto T = ev->evalV(r, filePos);
			assert(dyn_cast<PointerType>(T->getType()));
			auto NU = ConstantPointerNull::get((llvm::PointerType*) T->getType());
			if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), boolClass);
			else return new ConstantData(r.builder.CreateICmpNE(T,NU), boolClass);
		}
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_CPOINTER:{
		if(!( operation=="==" || operation=="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		auto T = value->getValue(r, filePos);
		Value* NU;
		if(dd->classType==CLASS_NULL){
			assert(dyn_cast<PointerType>(T->getType()));
			NU = ConstantPointerNull::get((llvm::PointerType*) T->getType());
		} else if(dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
			NU = ev->evaluate(r)->castToV(r, cc, filePos);
		} else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), boolClass);
		else return new ConstantData(r.builder.CreateICmpNE(T,NU), boolClass);
	}
	case CLASS_RATIONAL:{
		filePos.compilerError("Todo -- implement rationals");
		exit(1);
	}
	case CLASS_TUPLE:
	case CLASS_NAMED_TUPLE:{
		const TupleClass* t1 = (const TupleClass*)cc;
		const TupleClass* t2 = (const TupleClass*)dd;
		if(t1->innerTypes.size()!=t2->innerTypes.size() || (operation!="==" && operation!="!=")){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		const NamedTupleClass* nt1=(t1->classType==CLASS_NAMED_TUPLE)?((const NamedTupleClass*)cc):nullptr;
		const NamedTupleClass* nt2=(t2->classType==CLASS_NAMED_TUPLE)?((const NamedTupleClass*)dd):nullptr;
		if(nt1 && nt2){
			for(unsigned i=0; i<t1->innerTypes.size(); i++){
				if(nt1->innerNames[i]!=nt2->innerNames[i]){
					filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
					exit(1);
				}
			}
		}
		const Data* d = ev->evaluate(r);
		Value* V = ConstantInt::getTrue(getGlobalContext());
		for(unsigned i=0; i<t1->innerTypes.size(); i++){
			auto a1 = cc->getLocalData(r, filePos, (nt1)?(nt1->innerNames[i]):("_"+str(i)),value);
			auto a2 = dd->getLocalData(r, filePos, (nt2)?(nt2->innerNames[i]):("_"+str(i)),value);
			auto M = getBinop(r, filePos, a1, a2, operation);
			V = r.builder.CreateAnd(V,M->castToV(r, boolClass, filePos));
		}
		return new ConstantData(V, boolClass);
	}
	case CLASS_CLASS:
	case CLASS_FUNC:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		auto T = value->getValue(r, filePos);
		Value* NU;
		if(dd->classType==CLASS_NULL){
			assert(dyn_cast<PointerType>(T->getType()));
			NU = ConstantPointerNull::get((llvm::PointerType*) T->getType());
		} else if(dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
			NU = ev->evaluate(r)->castToV(r, cc, filePos);
		} else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), boolClass);
		else return new ConstantData(r.builder.CreateICmpNE(T,NU), boolClass);
	}
	case CLASS_ARRAY:{
		if(!(operation=="==" || operation=="!=") ||
				!(dd->classType==cc->classType|| dd->classType==CLASS_NULL || dd->classType==CLASS_CPOINTER)){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		const ArrayClass* a1 = (const ArrayClass*)cc;
		const ArrayClass* a2 = (const ArrayClass*)dd;
		if(a1->len!=a2->len || a1->inner!=a2->inner){
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		auto T = value->getValue(r, filePos);
		Value* NU;
		if(dd->classType==CLASS_NULL){
			assert(dyn_cast<PointerType>(T->getType()));
			NU = ConstantPointerNull::get((llvm::PointerType*) T->getType());
		} else if(dd->classType==CLASS_CLASS || dd->classType==CLASS_ARRAY || dd->classType==CLASS_CPOINTER || dd->layout==POINTER_LAYOUT || dd->layout==PRIMITIVEPOINTER_LAYOUT){
			NU = ev->evaluate(r)->castToV(r, cc, filePos);
		} else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
		if(operation=="==") return new ConstantData(r.builder.CreateICmpEQ(T,NU), boolClass);
		else return new ConstantData(r.builder.CreateICmpNE(T,NU), boolClass);
	}
	case CLASS_GEN:
	case CLASS_SET:
	case CLASS_MAP:
	case CLASS_AUTO:
	case CLASS_VOID:{
		if(false) return cc;
		else{
			filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
			exit(1);
		}
	}
	case CLASS_LAZY:
	case CLASS_REF:{
		filePos.error("Cannot apply binary operations to reference or lazy classes");
		exit(1);
	}
	case CLASS_USER:{
		const UserClass* uc = (const UserClass*)cc;
		return uc->getLocalFunction(filePos, ":"+operation, {value, ev})->callFunction(r, filePos, {value, ev});
	}
	}
}


#endif /* BINARY_HPP_ */
