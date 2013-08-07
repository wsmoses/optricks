/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../constructs/Expression.hpp"

class E_INDEXER : public Expression{
	public:
		Expression* array;
		Expression* index;
		E_INDEXER(Expression* t, Expression* ind) : Expression(t->returnType),array(t), index(ind){
		}
		const Token getToken() const override{
			return T_INDEXER;
		}
		oobject* evaluate() override final{
			//TODO allow short-circuit lookup of E_VAR
			return (*(array->evaluate()))[index->evaluate()];
		}
		Expression* simplify() override{
			Expression* a = array->simplify();
			Expression* b = index->simplify();
			if(a->getToken()==T_OOBJECT && b->getToken()==T_OOBJECT){
				return (*((oobject*)a))[(oobject*)b];
			}else{
				return new E_INDEXER(a,b);
			}
		}
		void write(ostream& f,String s="") const override{
			array->write(f,s);
			f << "[";
			index->write(f,s);
			f << "]";
		}
};


#endif /* E_INDEXER_HPP_ */
