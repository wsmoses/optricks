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
			oobject* a = array->evaluate();
			if(a->returnType != arrayClass){
				//TODO Implement custom lookup operator
				cerr << "Cannot lookup non-array";
				exit(0);
			}
			oarray* ar = (oarray*)a;
			return (*ar)[array->evaluate()];
		}
		Expression* simplify() override{
			Expression* b = array->evaluate();
			if(b->getToken()==T_OOBJECT){
				oobject* a =(oobject*)b;
				if(a->returnType != arrayClass){
					//TODO Implement custom lookup operator
					cerr << "Cannot lookup non-array";
					exit(0);
				}

				oarray* ar = (oarray*)a;
				Expression* ii = array->evaluate();
				if(ii->getToken()!=T_OOBJECT){
					return new E_INDEXER(b,ii);
				}
				oobject* i = (oobject*)ii;
				if(i->returnType != intClass){
					//TODO Implement custom lookup operator
					cerr << "Cannot lookup a non-int index ";
					exit(0);
				}
				//TODO remove error checking
				oint* j = ((oint*)i);
				if(j->value<0 && (unsigned int)j->value >=ar->data.size()){
					cerr << "Index out of range for length " << ar->data.size() << " index:" << j;
					exit(0);
				}
				return (*ar)[j];
			}else{
				return new E_INDEXER(b,index->simplify());
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
