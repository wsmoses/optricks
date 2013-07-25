/*
 * UO_NOT.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_NOT_HPP_
#define UO_NOT_HPP_

#include "../../../O_Expression.hpp"

class UO_NOT : public Expression{
public:
	Expression* value;
	UO_NOT(){ };
	UO_NOT(Expression* a){value = a;};
	~UO_NOT() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_NOT)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_NOT ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	ostream& write(ostream& f){
		f << "UO_NOT(";
		return value->write(f) << ")";
	}
};


#endif /* UO_NOT_HPP_ */
