/*
 * OBJ_INT.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_INT_HPP_
#define OBJ_INT_HPP_

#include "../O_Expression.hpp"

class OBJ_INT : public OBJ_OBJ{
public:
	long int value;
	OBJ_INT(){ value = 0;};
	OBJ_INT(long int a){value = a;};
	~OBJ_INT() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_INT)) return true;
		if(writeLong(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_INT ) return true;
		if(readLong(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_INT;
	}
	String toString(){
	    std::ostringstream ostr;
	    ostr << value;
	    return ostr.str();
	}

	ostream& write(ostream& f){
		return f << "INT(" << toString() << ')';
	}
};


#endif /* OBJ_INT_HPP_ */
