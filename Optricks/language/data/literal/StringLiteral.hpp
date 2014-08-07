/*
 * StringLiteral.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: Billy
 */

#ifndef STRINGLITERAL_HPP_
#define STRINGLITERAL_HPP_
#include "Literal.hpp"
#include "../../class/builtin/CharClass.hpp"
#include "../../class/literal/StringLiteralClass.hpp"

class StringLiteral: public Literal{
public:
	const String value;
	StringLiteral(const String val):Literal(R_STR),value(val){}
	const AbstractClass* getReturnType() const override final{
		if(value.length()==1) return &charClass;
		else return &stringLiteralClass;
	}
	bool hasCastValue(const AbstractClass* const a) const{
		if(value.length()==1) return charClass.hasCast(a);
		else return stringLiteralClass.hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
		if(value.length()==1) return charClass.compare(a,b);
		else return stringLiteralClass.compare(a,b);
	}
	//TODO
	llvm::Constant* getValue(RData& r, PositionID id) const override final{
		if(value.length()==1) return CharClass::getValue(value[0]);
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO allow cast to string (c_char ?)
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(value.size()==1 && right==&charClass)
			return new ConstantData(CharClass::getValue(value[0]), &charClass);
		if(right==&stringLiteralClass) return this;
		if(right->classType==CLASS_CSTRING){
			return new ConstantData(r.getConstantCString(value), right);
		}
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO
	llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		assert(right);
		if(value.size()==1 && right==&charClass)
			return CharClass::getValue(value[0]);
		if(right->classType==CLASS_CSTRING){
			return r.getConstantCString(value);
		}
		id.compilerError("String literal not implemented yet '"+value+"' "+right->getName()+" "+str(right->classType)+" "+((right==&charClass)?"true":"false"));
		exit(1);
	}
};



#endif /* STRINGLITERAL_HPP_ */
