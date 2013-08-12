/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_UOP_HPP_
#define E_UOP_HPP_

#include "../constructs/Expression.hpp"

class E_UOP : public Expression{
	public:
		const Token getToken() const override { return T_UOP; }
		Expression *value;
		String operation;
		E_UOP(String o, Expression* a): Expression(objectClass),
				value(a), operation(o)
		{};//possible refinement of return type
		oobject* comb(oobject* o){
			if(operation=="+"){
							return +(*o);
						} else if(operation=="-"){
							return -(*o);
						} else if(operation=="!"){
							return !(*o);
						} else if(operation=="~"){
							return ~(*o);
						}
						/*else if(operation=="&"){
							return -o;
						} else if(operation=="*"){
							return -o;
						}*/
						else {
							cerr << "Unknown unary operator: " << operation;
							exit(0);
						}
			return NULL;
		}
		Expression* simplify() override{
			Expression* a = value->simplify();
			if(a->getToken()==T_OOBJECT) return comb((oobject*)a);
			else return new E_UOP(operation,a);
		}
		oobject* evaluate() override{
			oobject* o = value->evaluate();
			return comb(o);
		}
		void write(ostream& f,String s="") const override{
			f << "(" << operation << value << ")";
		}
};



#endif /* E_BINOP_HPP_ */
