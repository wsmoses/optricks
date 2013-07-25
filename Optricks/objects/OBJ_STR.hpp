/*
 * OBJ_STR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_STR_HPP_
#define OBJ_STR_HPP_

#include "../O_Expression.hpp"

class OBJ_STR : public OBJ_OBJ{
public:
	String value;
	OBJ_STR(){};
	OBJ_STR(String a){value = a;};
	~OBJ_STR() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_STR)) return true;
		if(writeString(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_STR ) return true;
		if(readString(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_STR;
	}
	String toString(){
		return value;
	}
	ostream& write(ostream& f){
		return f << "STR('" << toString() << "')";
	}
};


#endif /* OBJ_STR_HPP_ */
