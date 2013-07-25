/*
 * E_ARR.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_ARR_HPP_
#define E_ARR_HPP_
#include "../O_Expression.hpp"

class E_ARR : public Expression{
public:
	std::vector<Expression*> values;
	E_ARR() : values() {};
	E_ARR(unsigned int len) : values(len) { };
	~E_ARR(){
		for (Expression* &it:  values)
			delete it;
	};
	bool writeBinary(FILE* f){
		writeByte(f, T_ARR);
		writeUInteger(f, values.size());
		for (Expression* &it:  values){
			if( it->writeBinary(f) ) return true;
		}
		return false;
	};
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f, &c)) return true;
		if(c!=T_ARR ) return true;
		unsigned int len;
		if(readUInteger(f,&len)) return true;
		values.clear();
		values.resize(len);
		for(unsigned int i = 0; i<len; ++i){
			Expression* temp;
			if(readExpression(f, &temp)) return true;
			values.push_back( temp );
		}
		return false;
	};
	Token getToken(){
		return T_ARR;
	};
	ostream& write(ostream& f){
		f << "E_ARR([";
		for(int i = 0; i<values.size(); ++i){
			f << values[i];
			if(i<values.size()-1) f << ", ";
		}
		return f << "])";
	}
};


#endif /* E_ARR_HPP_ */
