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
		E_BINOP(PositionID a, Statement* t, Statement* ind,String op) :
			Statement(a),left(t), right(ind),operation(op){
		}
		const Token getToken() const override{
			return T_BINOP;
		}
		Value* evaluate(RData& a) override final{
			//TODO allow short-circuit lookup of E_VAR
			auto temp = left->returnType->getBinop(filePos, operation, right->returnType);
			return temp.first->apply(
					temp.second.first->apply(left->evaluate(a), a),
					temp.second.second->apply(right->evaluate(a), a),
					a
			);
		}
		ClassProto* checkTypes() override final{
			auto leftT = left->checkTypes();
			auto rightT = right->checkTypes();
			auto temp = leftT->getBinop(filePos, operation, rightT);
			return returnType = temp.first->returnType;
		}
		Statement* simplify() override{
			return new E_BINOP(filePos, left->simplify(), right->simplify(), operation);
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

		//TODO CHECK IF WORKS
		Statement* fixOrderOfOperations(){
			Statement* tl = left;
			Statement* tr = right;
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
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); return NULL; }
};


#endif /* E_INDEXER_HPP_ */
