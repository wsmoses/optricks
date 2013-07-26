/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../O_Expression.hpp"

class E_INDEXER : public Expression{
public:
	Expression* array;
	Expression* index;
	E_INDEXER() {};
	E_INDEXER(Expression* t, Expression* ind) : array(t), index(ind){
	}
	~E_INDEXER(){
		delete array;
		delete index;
	};
	bool writeBinary(FILE* f){
		writeByte(f, T_INDEXER);
		array->writeBinary(f);
		index->writeBinary(f);
		return false;
	};
	bool readBinary(FILE* f){
		byte c;
		if(readByte(f, &c)) return true;
		if(c!=T_INDEXER ) return true;
		if(readExpression(f, &array)) return true;
		if(readExpression(f, &index)) return true;
		return false;
	};
	Token getToken(){
		return T_INDEXER;
	};
	ostream& write(ostream& f){
		f << "E_INDEXER(";
		array->write(f) << " ";
		index->write(f);

		return f << ")";
	}
};


#endif /* E_INDEXER_HPP_ */
