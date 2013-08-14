/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../constructs/Expression.hpp"


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
		Expression* left;
		Expression* right;
		String operation;
		E_BINOP(Expression* t, Expression* ind,String op) :
			Expression(t->returnType),left(t), right(ind),operation(op){
		}
		const Token getToken() const override{
			return T_BINOP;
		}
		Value* evaluate(RData& a,LLVMContext& context) override final{
			//TODO allow short-circuit lookup of E_VAR
			return left->returnType->binops[operation][right->returnType]->apply(
					left->evaluate(a,context),
					right->evaluate(a,context),
					a);
		}
		void checkTypes(){
			left->checkTypes();
			right->checkTypes();
			auto found = left->returnType->binops.find(operation);
			if(found==left->returnType->binops.end())
				todo("Binary operator ",operation," not implemented for class ",
						left->returnType->name, " [with right ", right->returnType->name,"]");
			auto look = found->second;

			auto found2 = look.find(right->returnType);
			if(found2==look.end())
				todo("Binary operator ",operation," not implemented for class ",
						left->returnType->name, " with right ", right->returnType->name);
		}
		Expression* simplify() override{
			return new E_BINOP(left->simplify(), right->simplify(), operation);
		}
		void write(ostream& f,String s="") const override{
			left->write(f,s);
			if(operation=="[]"){
			f << "[";
			right->write(f,s);
			f << "]";
			}
			else{
			f << " " << operation << " ";
			right->write(f,s);
			}
		}

		//TODO CHECK IF WORKS
		Expression* fixOrderOfOperations(){
			Expression* tl = left;
			Expression* tr = right;
			E_BINOP* self = this;
			while(true){
				if(tl->getToken()==T_BINOP){
					E_BINOP* l = (E_BINOP*)(tl);
					if(operation!="[]" && precedence(l->operation) > precedence(self->operation)){
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
					if(operation!="[]" && precedence(r->operation) > precedence(self->operation)){
						self->right = r->left;
						tl = r->left = self;
						self = r;
					}
					else break;
				} else break;
			}
			return self;
		}
};


#endif /* E_INDEXER_HPP_ */
