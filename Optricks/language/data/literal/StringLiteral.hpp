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
		cerr << "stringLiteral not implemented yet" << endl << flush;
		exit(1);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
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
	Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override final{
		id.error("String literal cannot be used as function");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("String literal cannot be used as class");
		exit(1);
	}
	//TODO
	Constant* getValue(RData& r, PositionID id) const override final{
		cerr << "stringLiteral not implemented yet" << endl << flush;
		exit(1);
	}
	//TODO allow cast to string, char, c_string (c_char ?)
	const StringLiteral* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		cerr << "stringLiteral not implemented yet" << endl << flush;
		exit(1);
	}
	//TODO
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		cerr << "stringLiteral not implemented yet" << endl << flush;
		exit(1);
	}
	void write(ostream& s, String st) const override final{
		s << "\"";
		for(unsigned i=0; i<st.length(); i++){
			auto const c=st[i];
			//if(c>255) s << "\x" << toStringHex(c); else
			if(c=='\\' || c=='"')
				s << "\\" << c;
			else if(c=='\n'){
				s << "\\n";
			} else if(c=='\t'){
				s << "\\t";
			} else if(c=='\a'){
				s << "\\a";
			//} else if(c=='\h'){
			//	s << "\\h";
			} else if(c=='\v'){
				s << "\\v";
			} else if(c=='\r'){
				s << "\\r";
			} else if(c=='\b'){
				s << "\\b";
			} else s << c;
		}
		s << "\"";
	}
};



#endif /* STRINGLITERAL_HPP_ */
