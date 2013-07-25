/*
 * E_VAR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_VAR_HPP_
#define E_VAR_HPP_

#include "../O_Expression.hpp"

class E_VAR : public Expression{
public:
	String name;
	unsigned int lock;
	E_VAR(){};
	E_VAR(String a){name = a; lock = 0;};
	E_VAR(String a, unsigned int l){name = a; lock = l;};
	~E_VAR() {};
	bool writeBinary(FILE* f){
		if(writeByte(f, T_VAR)) return true;
		if(writeString(f, name)) return true;
		if(writeUInteger(f, lock)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_VAR ) return true;
		if(readString(f,&name)) return true;
		if(readUInteger(f,&lock)) return true;
		return false;
	}
	Token getToken(){
		return T_VAR;
	}

	ostream& write(ostream& f){
		return f << "E_VAR('" << name << "')";
	}
};
#endif /* E_VAR_HPP_ */
