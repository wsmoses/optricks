/*
 * UO_POS.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_POS_HPP_
#define UO_POS_HPP_

#include "../../../O_Expression.hpp"

class UO_POS : public Expression{
public:
	Expression* value;
	UO_POS(){ };
	UO_POS(Expression* a){value = a;};
	~UO_POS() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_POS)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_POS ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	ostream& write(ostream& f){
		f << "UO_POS(";
		return value->write(f) << ")";
	}
};


#endif /* UO_POS_HPP_ */
