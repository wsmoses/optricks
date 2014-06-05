/*
 * E_INDEXER.hpp
 *
 *  Created on: Apr 12, 2013
 *      Author: wmoses
 */

#ifndef E_INDEXER_HPP_
#define E_INDEXER_HPP_
#include "../language/statement/Statement.hpp"
#include "../language/data/Data.hpp"
#include "../operators/Binary.hpp"
#include "./E_UOP.hpp"
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
private:
	E_BINOP(PositionID a, Statement* t, Statement* ind,String op) :
		ErrorStatement(a),left(t), right(ind),operation(op){
	}
public:
	Statement* left;
	Statement* right;
	String operation;
	virtual ~E_BINOP(){};
	const Token getToken() const override{
		return T_BINOP;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		assert(isClassMethod==false);
		auto type=getReturnType();
		if(type->classType==CLASS_FUNC){
			return ((FunctionClass*)type)->returnType;
		}  else if(type->classType==CLASS_LAZY){
			return ((LazyClass*)type)->innerType;
		//} else if(type->classType==CLASS_CLASS){
		//	return type;
		}	else {
			id.error("Class '"+type->getName()+"' cannot be used as function");
			exit(1);
		}
	}
	const Data* evaluate(RData& a) const override final{
		auto L = left->evaluate(a);
		return getBinop(a, filePos, L, right, operation);
	}
	const AbstractClass* getReturnType() const override final{
		auto LC = left->getReturnType();
		auto RC = right->getReturnType();
		return getBinopReturnType(filePos, LC, RC, operation);
	}
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
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

	const AbstractClass* getMyClass(RData& r, PositionID id, const std::vector<TemplateArg>& args)const{
		id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
	}
	static Statement* createBinop(PositionID id, Statement* L, Statement* R, String operation){
		if(operation=="[]" || operation=="[]=") return new E_BINOP(id, L, R, operation);
		if(operation=="**"){
			if(R->getToken()==T_BINOP){
				E_BINOP* RB = (E_BINOP*)R;
				//cerr << RB->operation << "|&" << endl << flush;
				if(RB->operation=="**"){
					E_BINOP* C = (new E_BINOP(id, L, R, "**"));
					//LB->right = C;
					if(L->getToken()==T_UOP){
						E_UOP* LU = (E_UOP*)L;
						//cerr << LU->operation << "|" << endl << flush;
						if(LU->pre==UOP_PRE){
							C->left = LU->value;
							LU->value = C->fixOrderOfOperations();
							return LU;
						}
					} else if(L->getToken()==T_LITERAL){
						Data* D = (Data*)L;
						if(D->type==R_INT){
							IntLiteral* IL = (IntLiteral*)D;
							if(mpz_sgn(IL->value)<0){
								mpz_t n;
								mpz_init(n);
								mpz_neg(n, IL->value);
								D = new IntLiteral(n);
								mpz_clear(n);
								C->left = D;
								return new E_UOP(id, "-",C, UOP_PRE);
							}
						} else if(D->type==R_FLOAT){
							FloatLiteral* IL = (FloatLiteral*)D;
							if(mpfr_sgn(IL->value)<0){
								mpfr_t n;
								mpfr_init(n);
								mpfr_neg(n, IL->value, MPFR_RNDN);
								D = new FloatLiteral(n);
								mpfr_clear(n);
								C->left = D;
								return new E_UOP(id, "-",C, UOP_PRE);
							}
						}
					}
					return C;
				}
			}
			if(L->getToken()==T_BINOP){
				E_BINOP* LB = (E_BINOP*)L;
				//cerr << LB->operation << "|" << endl << flush;
				if(LB->operation=="**"){
					Statement* C = (new E_BINOP(id, LB->left, R, "**"))->fixOrderOfOperations();
					LB->right = C;
					return LB;
				}
			}
			if(L->getToken()==T_UOP){
				E_UOP* LU = (E_UOP*)L;
				//cerr << LU->operation << "|" << endl << flush;
				if(LU->pre==UOP_PRE){
					Statement* C = (new E_BINOP(id, LU->value, R, "**"))->fixOrderOfOperations();
					LU->value = C;
					return LU;
				}
			} else if(L->getToken()==T_LITERAL){
				Data* D = (Data*)L;
				if(D->type==R_INT){
					IntLiteral* IL = (IntLiteral*)D;
					if(mpz_sgn(IL->value)<0){
						mpz_t n;
						mpz_init(n);
						mpz_neg(n, IL->value);
						D = new IntLiteral(n);
						mpz_clear(n);
						return new E_UOP(id, "-",(new E_BINOP(id, D, R, "**"))->fixOrderOfOperations(), UOP_PRE);
					}
				} else if(D->type==R_FLOAT){
					FloatLiteral* IL = (FloatLiteral*)D;
					if(mpfr_sgn(IL->value)<0){
						mpfr_t n;
						mpfr_init(n);
						mpfr_neg(n, IL->value, MPFR_RNDN);
						D = new FloatLiteral(n);
						mpfr_clear(n);
						return new E_UOP(id, "-",(new E_BINOP(id, D, R, "**"))->fixOrderOfOperations(), UOP_PRE);
					}
				}
			}
		}
		return (new E_BINOP(id, L, R, operation))->fixOrderOfOperations();
	}
	Statement* fixOrderOfOperations(){
		if(operation=="[]"|| operation=="[]=") return this;
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
