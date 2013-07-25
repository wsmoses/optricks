/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../O_Expression.hpp"

class E_LOOKUP : public Expression{
	public:
		Token getToken(){ return T_LOOKUP; }
		Expression* left;
		E_VAR* right;
		String operation;
		E_LOOKUP(String o, Expression* a, E_VAR* b): left(a), right(b), operation(o){};
		E_LOOKUP(){}

		bool writeBinary(FILE* f){
			return writeByte(f, T_LOOKUP);
			if(writeString(f, operation)) return true;
			if(left->writeBinary(f)) return true;
			if(right->writeBinary(f)) return true;
			return false;
		}
		bool readBinary(FILE* f){
			byte c;
			if(readByte(f,&c)) return true;
			if(c!=T_LOOKUP) return true;
			if(readString(f, &operation)) return true;
			if(readExpression(f, &left)) return true;
			Expression* r;
			if(readExpression(f, &r)) return true;
			if(r->getToken()!=T_VAR) return true;
			right = (E_VAR*)r;
			return false;
		}
		ostream& write(ostream& f){
			f << "E_LOOKUP('" << operation << "', ";
			left->write(f);
			f << ", ";
			return right->write(f) << ")";
		}
};



#endif /* E_LOOKUP_HPP_ */
