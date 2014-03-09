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
				filePos.error("Math literal class cannot combine with integer types directly -- cast to floating-point type first");
				exit(1);
			}
			default:{
				filePos.error("Could not find binary operation '"+operation+"' between class '"+cc->getName()+"' and '"+dd->getName()+"'");
				exit(1);
			}
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
		case CLASS_STR:
		case CLASS_CHAR:{

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

inline const AbstractClass* getBinop(RData&, PositionID filePos, const Data* value, const Evaluatable* ev, const String operation){
	const AbstractClass* cc = value->getReturnType();
}


#endif /* BINARY_HPP_ */
