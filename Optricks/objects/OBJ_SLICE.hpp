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
	OBJ_SLICE(int){};
	OBJ_SLICE(Expression* a= NULLV,Expression* e = NULLV, Expression* s=NULLV)
	{start = a; end = e; step = s;};
	~OBJ_SLICE() {};
	bool writeBinary(FILE* f){
		if(writeByte(f,T_SLICE)) return true;
		if(start->writeBinary(f)) return true;
		if(end->writeBinary(f)) return true;
		if(step->writeBinary(f)) return true;
		return false;
	}
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f,&c)) return true;
		if(c!=T_SLICE ) return true;
		if(readExpression(f,&start)) return true;
		if(readExpression(f,&end)) return true;
		if(readExpression(f,&step)) return true;
		return false;
	}
	Token getToken(){
		return T_SLICE;
	}
	String toString(){
	    std::ostringstream ostr;
	    ostr << "slice(" << start << "," << end <<","<< step<<")";
	    return ostr.str();
	}

	ostream& write(ostream& f){
		return f << toString() ;
	}
};


#endif /* OBJ_SLICE_HPP_ */
