/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../constructs/Statement.hpp"

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

class E_BINOP : public Statement{
	public:
		Statement* left;
		Statement* right;
		String operation;
		virtual ~E_BINOP(){};
		E_BINOP(PositionID a, Statement* t, Statement* ind,String op) :
			Statement(a),left(t), right(ind),operation(op){
		}
		const Token getToken() const override{
			return T_BINOP;
		}
		DATA evaluate(RData& a) override final{
			//TODO allow short-circuit lookup of E_VAR
			auto temp = left->returnType->getBinop(filePos, operation, right->returnType);
			return temp.first->apply(
					temp.second.first->apply(left->evaluate(a), a),
					temp.second.second->apply(right->evaluate(a), a),
					a
			);
		}
		ClassProto* checkTypes(RData& r) override final{
			auto leftT = left->checkTypes(r);
			auto rightT = right->checkTypes(r);
			auto temp = leftT->getBinop(filePos, operation, rightT);
			return returnType = temp.first->returnType;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
		Constant* getConstant(RData& r) override final{ return NULL; }//TODO allow constant folding
		Statement* simplify() override{
			return new E_BINOP(filePos, left->simplify(), right->simplify(), operation);
		}
		ClassProto* getSelfClass() override final{
			error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL;
		}
		void registerClasses(RData& r) override final{
			left->registerClasses(r);
			right->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			left->registerFunctionArgs(r);
			right->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			left->registerFunctionDefaultArgs();
			right->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			left->resolvePointers();
			right->resolvePointers();
		};
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

		Statement* fixOrderOfOperations(){
			if(operation=="[]") return this;
			Statement* tl = left;
			Statement* tr = right;
			E_BINOP* self = this;
			while(true){
				if(tl->getToken()==T_BINOP){
					E_BINOP* l = (E_BINOP*)(tl);
					if(l->operation[0]=='[') break;
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
					if(r->operation[0]=='[') break;
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
		String getFullName() override final{
			error("Cannot get full name of binop");
			return "";
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_BINOP");
			return NULL;
		}
};


#endif /* E_INDEXER_HPP_ */
