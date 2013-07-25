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
	E_ARR* index;
	E_INDEXER() {};
	E_INDEXER(Expression* t, E_ARR* ar) : array(t), index(ar){
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
		Expression* e;
		if(readExpression(f, &e)) return true;
		if(e->getToken()!=T_ARR) return true;
		index = (E_ARR*)e;
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
