/*
 * UO_REF.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_REF_HPP_
#define UO_REF_HPP_

#include "../../../O_Expression.hpp"

class UO_REF : public Expression{
public:
	Expression* value;
	UO_REF(){ };
	UO_REF(Expression* a){value = a;};
	~UO_REF() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_REF)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_REF ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	ostream& write(ostream& f){
		f << "UO_REF(";
		return value->write(f) << ")";
	}
};


#endif /* UO_REF_HPP_ */
