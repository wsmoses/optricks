/*
 * OBJ_SLICE.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_SLICE_HPP_
#define OBJ_SLICE_HPP_

#include "../O_Expression.hpp"

class OBJ_SLICE : public OBJ_OBJ{
public:
	Expression *start, *end, *step;
	OBJ_SLICE(){};
	OBJ_SLICE(long SLICE a){value = a;};
	~OBJ_SLICE() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_SLICE)) return true;
		if(writeLong(f, value)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_SLICE ) return true;
		if(readLong(f,&value)) return true;
		return false;
	}
	Token getToken(){
		return T_SLICE;
	}
	String toString(){
	    std::ostringstream ostr;
	    ostr << value;
	    return ostr.str();
	}

	ostream& write(ostream& f){
		return f << "SLICE(" << toString() << ')';
	}
};


#endif /* OBJ_SLICE_HPP_ */
