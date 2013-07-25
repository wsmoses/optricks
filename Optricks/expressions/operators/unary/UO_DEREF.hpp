/*
 * UO_DEREF.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef UO_DEREF_HPP_
#define UO_DEREF_HPP_

#include "../../../O_Expression.hpp"

class UO_DEREF : public Expression{
public:
	Expression* value;
	UO_DEREF(){ };
	UO_DEREF(Expression* a){value = a;};
	~UO_DEREF() { delete value; };
	bool writeBinary(FILE* f){
		if(writeByte(f,T_DEREF)) return true;
		if(value->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_DEREF ) return true;
		if(readExpression(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	ostream& write(ostream& f){
		f << "UO_DEREF(";
		return value->write(f) << ")";
	}
};


#endif /* UO_DEREF_HPP_ */
