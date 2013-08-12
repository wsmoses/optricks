/*
 * OBJ_KEY_FUNC.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_KEY_FUNC_HPP_
#define OBJ_KEY_FUNC_HPP_

#include "DefaultDeclaration.hpp"
#include "../O_Expression.hpp"

class OBJ_KEY_FUNC : public Expression{
public:
	OBJ_KEY_FUNC(){};
	~OBJ_KEY_FUNC() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_KEY_FUNC)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_KEY_FUNC ) return true;
		return false;
	}
	Token getToken(){
		return T_KEY_FUNC;
	}
	ostream& write(ostream& f){
		return f << "OBJ_KEY_FUNC()";
	}
};


#endif /* OBJ_KEY_FUNC_HPP_ */
