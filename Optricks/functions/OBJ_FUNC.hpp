/*
 * OBJ_FUNC.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_FUNC_HPP_
#define OBJ_FUNC_HPP_

#include "OBJ_KEY_FUNC.hpp"
#include "../O_Expression.hpp"

class OBJ_FUNC : public Expression{
public:
		std::vector<DefaultDeclaration*> arguments;
		Expression* body;
		String name;
		OBJ_FUNC(String n, std::vector<DefaultDeclaration*> arg, Expression* b):arguments(arg),body(b),name(n){

		}
	OBJ_FUNC():arguments(),body(NULL),name(){};
	~OBJ_FUNC() {delete body;};
	//TODO add
	bool writeBinary(FILE* f){
		if(writeByte(f,T_FUNC)) return true;
		return false;
	}
	//TODO add
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_FUNC ) return true;
		return false;
	}
	Token getToken(){
		return T_FUNC;
	}
	ostream& write(ostream& f){
		f << "OBJ_FUNC('" << name << "', [";
		for(unsigned int i = 0; i<arguments.size(); ++i){
			f << arguments[i];
			if(i<arguments.size()-1) f << ", ";
		}
		f << "], ";
		return body->write(f) << ")";
	}
};


#endif /* OBJ_FUNC_HPP_ */
