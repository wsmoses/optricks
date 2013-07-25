/*
 * OBJ_NULL.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_NULL_HPP_
#define OBJ_NULL_HPP_

#include "OBJ_OBJ.hpp"

class OBJ_NULL : public OBJ_OBJ{
public:
	OBJ_NULL(){};
	~OBJ_NULL() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_NULL)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_NULL ) return true;
		return false;
	}
	Token getToken(){
		return T_NULL;
	}
	String toString(){
	    return "NULL";
	}

	ostream& write(ostream& f){
		return f << "NULL";
	}
};


#endif /* OBJ_NULL_HPP_ */
