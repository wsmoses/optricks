/*
 * OBJ_ARR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef OBJ_ARR_HPP_
#define OBJ_ARR_HPP_

#include "../O_Expression.hpp"

class OBJ_ARR : public OBJ_OBJ{
public:
		std::vector<Expression*> values;
	OBJ_ARR() : values() {};
	OBJ_ARR(unsigned int len): values(len) {};
	~OBJ_ARR() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_O_ARR)) return true;
		if(writeUInteger(f, values.size())) return true;
		for(auto &a: values){
			if(a->writeBinary(f)) return true;
		}
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_O_ARR ) return true;
		unsigned int len;
		if(readUInteger(f,&len)) return true;
		values.resize(len);
		for(unsigned int i = 0; i<len; i++){
			if(readExpression(f, (Expression**)&values[i])) return true;
		}
		return false;
	}
	Token getToken(){
		return T_ARR;
	}
	String toString(){
		std::stringstream ss;
		ss << "[";
		if(values.size()>0) {
		for(unsigned int i = 0; i<values.size(); ++i){
			ss << values[i];
			if(i<values.size()-1) ss << ", ";
		}
		}
		ss << "]";
		return ss.str();
	}
};


#endif /* OBJ_ARR_HPP_ */
