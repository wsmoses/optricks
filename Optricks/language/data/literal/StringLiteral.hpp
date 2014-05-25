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
//TODO
class StringLiteral: public Literal{
public:
	const String value;
	StringLiteral(const String val):Literal(R_STR),value(val){}
	//TODO
	const AbstractClass* getReturnType() const override final{
		if(value.length()==1) return &charClass;
		else return &stringLiteralClass;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("String literal cannot act as function");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const{
		if(value.length()==1) return charClass.hasCast(a);
		else return stringLiteralClass.hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
		if(value.length()==1) return charClass.compare(a,b);
		else return stringLiteralClass.compare(a,b);
	}
	Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override final{
		id.error("String literal cannot be used as function");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("String literal cannot be used as class");
		exit(1);
	}
	//TODO
	llvm::Constant* getValue(RData& r, PositionID id) const override final{
		if(value.length()==1) return CharClass::getValue(value[0]);
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO allow cast to string, char, c_string (c_char ?)
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(value.size()==1 && right->classType==CLASS_CHAR)
			return new ConstantData(CharClass::getValue(value[0]), &charClass);
		if(right==&stringLiteralClass) return this;
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO
	llvm::Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(value.size()==1 && right->classType==CLASS_CHAR)
			return CharClass::getValue(value[0]);
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
};



#endif /* STRINGLITERAL_HPP_ */
