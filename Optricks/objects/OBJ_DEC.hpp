/*
 * OBJ_DEC.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_DEC_HPP_
#define OBJ_DEC_HPP_

#include "../O_Expression.hpp"

class OBJ_DEC : public OBJ_OBJ{
public:
	double value;
	OBJ_DEC(){ value = 0;};
	OBJ_DEC(double a){value = a;};
	~OBJ_DEC() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_INT)) return true;
		if(writeDouble(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_INT ) return true;
		if(readDouble(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_DEC;
	}
	String toString(){
	    std::ostringstream ostr;
	    ostr << value;
	    return ostr.str();
	}
};


#endif /* OBJ_DEC_HPP_ */
