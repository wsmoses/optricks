/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_BINOP_HPP_
#define E_BINOP_HPP_

#include "../constructs/Expression.hpp"
#include "../primitives/oobjectproto.hpp"
byte precedence(String tmp){
	if (tmp == "." || tmp==":" || tmp=="::" || tmp == "->"){
		return 0;
	}
	else if(tmp=="++" || tmp == "--" || tmp=="%%"){
		return 1;
	}
	else if(tmp==".*" || tmp==":*" || tmp=="::*"||tmp=="->*" || tmp=="=>*"){
		return 2;
	}
	else if (tmp == "**" || tmp=="^"){
		return 3;
	}
	else if (tmp == "%" || tmp == "*" || tmp == "/" || tmp == "//"){
		return 4;
	}
	else if (tmp == "+" || tmp == "-"){
		return 5;
	}
	else if(tmp=="<<" || tmp == ">>"){
		return 6;
	}
	else if (tmp == "<=" || tmp == "<" || tmp == ">" || tmp == ">="){
		return 7;
	}
	else if (tmp=="==" || tmp=="===" || tmp == "!=" || tmp=="!=="){
		return 8;
	}
	else if (tmp == "&"){
		return 9;
	}
	else if (tmp == "^^"){
		return 10;
	}
	else if (tmp == "|"){
		return 11;
	}
	else if (tmp == "&&"){
		return 12;
	}
	else if (tmp == "||"){
		return 13;
	}

	else if(tmp == "\\."){
		return 14;
	}
	else if (tmp == "+=" || tmp == "%="
			|| tmp == "-=" || tmp == "*="
					|| tmp=="\\.=" || tmp == "\\=" || tmp=="/=" || tmp=="//="
							|| tmp == "**=" || tmp=="^=" || tmp=="|=" || tmp=="||="
									|| tmp=="&=" || tmp=="&&=" || tmp=="^^=" || tmp=="="
											|| tmp == ":=" || tmp == "::=" ||tmp == "<<=" || tmp==">>="
													|| tmp=="<<<=" || tmp==">>>="){
		return 15;
	}

	return 255;
}

class E_BINOP : public Expression{
	public:
		const Token getToken() const override{ return T_BINOP; }
		Expression *left, *right;
		String operation;
		E_BINOP(String o, Expression* a, Expression* b): Expression(objectClass),
				left(a), right(b), operation(o)
				{
				};//possible refinement of return type


		//TODO CHECK IF WORKS
		Expression* fixOrderOfOperations(){
			Expression* tl = left;
			Expression* tr = right;
			E_BINOP* self = this;
			while(true){
				if(tl->getToken()==T_BINOP){
					E_BINOP* l = (E_BINOP*)(tl);
					if(precedence(l->operation) > precedence(self->operation)){
						self->left = l->right;
						tr = l->right = self;
						self = l;
					}
					else break;
				} else break;
			}

			while(true){
				if(tr->getToken()==T_BINOP){
					E_BINOP* r = (E_BINOP*)(tr);
					if(precedence(r->operation) > precedence(self->operation)){
						self->right = r->left;
						tl = r->left = self;
						self = r;
					}
					else break;
				} else break;
			}
			return self;
		}
		oobject* combine(oobject* a, oobject* b){
			//TODO allow other data types
			if(operation=="+") 		return *a + b;
			if(operation=="+=") 	return *a += b;
			if(operation=="-") 		return *a - b;
			if(operation=="-=")		return *a-=b;
			if(operation=="*") 		return *a * b;
			if(operation=="*=") 	return *a *= b;
			if(operation=="/") 		return *a / b;
			if(operation=="/=") 	return *a /= b;
			if(operation=="^") 		return *a ^ b;
			if(operation=="^=") 	return *a ^= b;
			if(operation=="%") 		return *a % b;
			if(operation=="%=") 	return *a %= b;
			if(operation=="!=") 	return *a != b;
			if(operation=="==") 	return *a == b;
			if(operation=="<") 		return *a < b;
			if(operation=="<=") 	return *a <= b;
			if(operation==">") 		return *a > b;
			if(operation==">=") 	return *a >= b;
			if(operation=="|") 		return *a | b;
			if(operation=="|=") 	return *a |= b;
			if(operation=="||") 	return *a || b;
			if(operation=="&") 		return *a & b;
			if(operation=="&=") 	return *a &= b;
			if(operation=="&&") 	return *a && b;
			if(operation=="=") 		return *a = b;
			if(operation=="<<") 	return *a << b;
			if(operation==">>") 	return *a >> b;
			if(operation=="<<=") 	return *a <<= b;
			if(operation==">>=") 	return *a >>= b;
			else{
				cerr << "Operation not allowed by language";
				exit(1);
			}
		}
		Expression* simplify() override{
			Expression* a = left->simplify();
			Expression* b = right->simplify();
			if(a->getToken()==T_OOBJECT && b->getToken()==T_OOBJECT)
				return combine((oobject*)a,(oobject*)b);
			else return new E_BINOP(operation,a,b);
		}
		oobject* evaluate() override{
			return combine(left->evaluate(),right->evaluate());
		}
		void write(ostream& f,String s="") const override{
			f << left << operation << right;
		}
};



#endif /* E_BINOP_HPP_ */
