/*
 * UO_NEG.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_NEG_HPP_
#define UO_NEG_HPP_

#include "../../../O_Expression.hpp"

class UO_NEG : public Expression{
public:
	Expression* value;
	UO_NEG(){ };
	UO_NEG(Expression* a){value = a;};
	~UO_NEG() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_NEG)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_NEG ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	ostream& write(ostream& f){
		f << "UO_NEG(";
		return value->write(f) << ")";
	}
};


#endif /* UO_NEG_HPP_ */
