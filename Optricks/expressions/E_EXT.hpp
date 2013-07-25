/*
 * E_EXT.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_EXT_HPP_
#define E_EXT_HPP_
#include "../O_Expression.hpp"

class E_EXT : public Expression{
public:
	String value;
	E_EXT(){};
	E_EXT(String a){value = a;};
	~E_EXT() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_EXT)) return true;
		if(writeString(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_EXT ) return true;
		if(readString(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_EXT;
	}
	ostream& write(ostream& f){
		return f << "E_EXT('" << value << "')";
	}
};

#endif /* E_EXT_HPP_ */
