/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../language/Operators.hpp"
#include "../language/statement/Statement.hpp"
#include "../language/data/Data.hpp"
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

class E_BINOP : public ErrorStatement{
	public:
		Statement* left;
		Statement* right;
		String operation;
		virtual ~E_BINOP(){};
		E_BINOP(PositionID a, Statement* t, Statement* ind,String op) :
			ErrorStatement(a),left(t), right(ind),operation(op){
		}
		const Token getToken() const override{
			return T_BINOP;
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
					auto type=getReturnType();
					if(type->classType==CLASS_FUNC){
									return ((FunctionClass*)type)->returnType;
								}  else if(type->classType==CLASS_CLASS){
									return type;
								}	else {
									id.error("Class '"+type->getName()+"' cannot be used as function");
									exit(1);
								}
				}
		const Data* evaluate(RData& a) const override final{
			auto tmp = OPERATOR_MAP.find(operation);
			if(tmp==OPERATOR_MAP.end()){
				filePos.error("Could not find binary operator "+operation);
				exit(1);
			}
			return tmp->second.callFunction(a, filePos, {left, right});
		}
		const AbstractClass* getReturnType() const override final{
			auto tmp = OPERATOR_MAP.find(operation);
			if(tmp==OPERATOR_MAP.end()){
				filePos.error("Could not find binary operator "+operation);
				exit(1);
			}
			return tmp->second.getFunctionReturnType(filePos, {left, right});
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		Statement* simplify() override{
			return new E_BINOP(filePos, left->simplify(), right->simplify(), operation);
		}
		void registerClasses() const override final{
			left->registerClasses();
			right->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			left->registerFunctionPrototype(r);
			right->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) const override final{
			left->buildFunction(r);
			right->buildFunction(r);
		};
		void write(ostream& f,String s="") const override{
			if(operation!="[]") f << "(";
			left->write(f,s);
			if(operation=="[]"){
				f << "[";
				right->write(f,s);
				f << "]";
			}
			else{
				f << " " << operation << " ";
				right->write(f,s);
				f << ")";
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
						tr = l->right = self->fixOrderOfOperations();
						self = l;
						tl = self->left;
					}
					else break;
				} else break;
			}

			while(true){
				if(tr->getToken()==T_BINOP){
					E_BINOP* r = (E_BINOP*)(tr);
					if(r->operation[0]=='[') break;
					if(precedence(r->operation) >= precedence(self->operation)){
						self->right = r->left;
						tl = r->left = self->fixOrderOfOperations();
						self = r;
						tr = self->right;
					}
					else break;
				} else break;
			}
			return self;
		}
};


#endif /* E_INDEXER_HPP_ */
