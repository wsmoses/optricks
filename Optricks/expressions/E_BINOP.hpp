/*
 * E_BINOP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_BINOP_HPP_
#define E_BINOP_HPP_

#include "../O_Expression.hpp"

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

class BinaryOperator : public Expression{
	public:
		Token getToken(){ return T_BINOP; }
		Expression *left, *right;
		BinaryOperator(String o, Expression* a, Expression* b): left(a), right(b), operation(o){};
		String operation;

		bool writeBinary(FILE* f){
			return writeByte(f, T_BINOP);
			if(writeString(f, operation)) return true;
			if(left->writeBinary(f)) return true;
			if(right->writeBinary(f)) return true;
			return false;
		}
		bool readBinary(FILE* f){
			byte c;
			if(readByte(f,&c)) return true;
			if(c!=T_BINOP) return true;
			if(readString(f, &operation)) return true;
			if(readExpression(f, &left)) return true;
			if(readExpression(f, &right)) return true;
			return false;
		}
		//TODO CHECK IF WORKS
		Expression* fixOrderOfOperations(){
			Expression* tl = left;
			Expression* tr = right;
			BinaryOperator* self = this;
			while(true){
				if(tl->getToken()==T_BINOP){
					BinaryOperator* l = (BinaryOperator*)(tl);
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
					BinaryOperator* r = (BinaryOperator*)(tr);
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
		ostream& write(ostream& f){
			f << "E_Binop('" << operation << "', ";
			left->write(f);
			f << ", ";
			return right->write(f) << ")";
		}
};



#endif /* E_BINOP_HPP_ */
