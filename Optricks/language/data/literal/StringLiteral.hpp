/*
 * StringLiteral.hpp
 *
 *  Created on: Dec 28, 2013
 *      Author: Billy
 */

#ifndef STRINGLITERAL_HPP_
#define STRINGLITERAL_HPP_
#include "Literal.hpp"
//TODO
class StringLiteral: public Literal{
public:
	const String value;
	StringLiteral(const String val):Literal(R_STR),value(val){}
	//TODO
	const AbstractClass* getReturnType() const override final{
		PositionID(0,0,"#stringLiteral").compilerError("stringLiteral not implemented yet");
		exit(1);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("String literal cannot act as function");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const{
		if(a->classType==CLASS_STR) return true;
		else return a->classType==CLASS_CHAR && value.length()==1;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
		PositionID(0,0,"#string").compilerError("StringLiteral not complete");
		exit(1);
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
	Constant* getValue(RData& r, PositionID id) const override final{
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO allow cast to string, char, c_string (c_char ?)
	const StringLiteral* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
	//TODO
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		id.compilerError("String literal not implemented yet");
		exit(1);
	}
};



#endif /* STRINGLITERAL_HPP_ */
