/*
 * E_SEP.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_SEP_HPP_
#define E_SEP_HPP_
#include "../O_Expression.hpp"

class E_SEP : public Expression{
public:
	char value;
	E_SEP(){};
	E_SEP(char a){value = a;};
	~E_SEP() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_SEP)) return true;
		if(writeChar(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_SEP ) return true;
		if(readChar(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_SEP;
	}
	ostream& write(ostream& f){
		f << "E_SEP('" ;
		f.put(value);
		return f << "')";
	}
};

#endif /* E_SEP_HPP_ */
