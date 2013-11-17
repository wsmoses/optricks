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
			if(left->returnType == right->returnType && left->returnType==boolClass && (operation=="&&" || operation=="||")){
				Value* Start = left->evaluate(a).getValue(a,filePos);
				BasicBlock* StartBB = a.builder.GetInsertBlock();
				BasicBlock *ElseBB;
				BasicBlock *MergeBB;
				if(operation=="&&"){
					if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
						if(c->isOne()) return right->evaluate(a).toValue(a,filePos);
						else return DATA::getConstant(getBool(false),boolClass);
					}
					else{
						ElseBB = a.CreateBlock("else",StartBB);
						MergeBB = a.CreateBlock("ifcont",StartBB/*,ElseBB*/);
						a.builder.CreateCondBr(Start, ElseBB, MergeBB);
					}
					StartBB = a.builder.GetInsertBlock();
					a.builder.SetInsertPoint(ElseBB);
					Value* fin = right->evaluate(a).getValue(a,filePos);
					//TODO can allow check if right is constant
					a.builder.CreateBr(MergeBB);
					ElseBB = a.builder.GetInsertBlock();
					//a.addPred(MergeBB,ElseBB);
					a.builder.SetInsertPoint(MergeBB);
					PHINode *PN = a.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
					PN->addIncoming(Start, StartBB);
					PN->addIncoming(fin, ElseBB);
					return DATA::getConstant(PN, boolClass);
				}else{
					if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
						if(! c->isOne()) right->evaluate(a).getValue(a,filePos);
						else return DATA::getConstant(getBool(true),boolClass);
					}
					else{
						ElseBB = a.CreateBlock("else",StartBB);
						MergeBB = a.CreateBlock("ifcont",StartBB/*,ElseBB*/);
						a.builder.CreateCondBr(Start, MergeBB, ElseBB);
					}
					StartBB = a.builder.GetInsertBlock();
					a.builder.SetInsertPoint(ElseBB);
					Value* fin = right->evaluate(a).getValue(a,filePos);
					//TODO can allow check if right is constant
					a.builder.CreateBr(MergeBB);
					ElseBB = a.builder.GetInsertBlock();
					//a.addPred(MergeBB,ElseBB);
					a.builder.SetInsertPoint(MergeBB);
					PHINode *PN = a.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
					PN->addIncoming(Start, StartBB);
					PN->addIncoming(fin, ElseBB);
					return DATA::getConstant(PN, boolClass);
				}
			}
			auto temp = left->returnType->getBinop(filePos, operation, right->returnType);
			const DATA p1 = temp.second.first->apply(left->evaluate(a), a,filePos);
			const DATA p2 = temp.second.second->apply(right->evaluate(a), a,filePos);
			DATA nex = temp.first->apply(p1,p2,a,filePos);
			return nex;
		}
		ClassProto* checkTypes(RData& r) override final{
			auto leftT = left->checkTypes(r);
			auto rightT = right->checkTypes(r);
			if(leftT == rightT && leftT==boolClass && (operation=="&&" || operation=="||"))
				return returnType=boolClass;
			auto temp = leftT->getBinop(filePos, operation, rightT);
			return returnType = temp.first->returnType;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe) override final{
		}
		Statement* simplify() override{
			return new E_BINOP(filePos, left->simplify(), right->simplify(), operation);
		}
		void registerClasses(RData& r) override final{
			left->registerClasses(r);
			right->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			left->registerFunctionPrototype(r);
			right->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) override final{
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
