/*
 * UO_BNOT.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_BNOT_HPP_
#define UO_BNOT_HPP_

#include "../../../O_Expression.hpp"

class UO_BNOT : public Expression{
public:
	Expression* value;
	UO_BNOT(){ };
	UO_BNOT(Expression* a){value = a;};
	~UO_BNOT() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_BNOT)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	ostream& write(ostream& f){
		f << "UO_BNOT(";
		return value->write(f) << ")";
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_BNOT ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
};


#endif /* UO_BNOT_HPP_ */
