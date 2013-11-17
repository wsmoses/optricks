/*
 * TernaryOperator.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef TERNARYOPERATOR_HPP_
#define TERNARYOPERATOR_HPP_

#include "../constructs/Statement.hpp"

class TernaryOperator : public Statement{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		virtual ~TernaryOperator(){};
		TernaryOperator(PositionID a, Statement* cond, Statement* th, Statement* const stat) :
			Statement(a), condition(cond), then(th), finalElse(stat){
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		String getFullName() override final{
			error("Cannot get full name of ternary");
			return "";
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_TERNARY");
			return NULL;
		}
		const Token getToken() const override {
			return T_TERNARY;
		}

		void registerClasses(RData& r) override final{
			condition->registerClasses(r);
			then->registerClasses(r);
			finalElse->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			condition->registerFunctionPrototype(r);
			then->registerFunctionPrototype(r);
			finalElse->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			condition->buildFunction(r);
			then->buildFunction(r);
			finalElse->buildFunction(r);
		}
		ClassProto* checkTypes(RData& r) override{
			if(condition->checkTypes(r)!=boolClass) error("Cannot have non-bool as condition for ternary "+condition->returnType->name);
			ClassProto* g = then->checkTypes(r);
			ClassProto* b = finalElse->checkTypes(r);
			ClassProto* tog = g->leastCommonAncestor(b,filePos);
			if(tog==NULL || tog==autoClass) error("Need matching types for ternary operator "+g->name+" and "+ b->name);
			return returnType = tog;
		}
		DATA evaluate(RData& r) override{
			Value* cond = condition->evaluate(r).getValue(r,filePos);
			if(ConstantInt* c = dyn_cast<ConstantInt>(cond)){
				if(c->isOne()){
					return then->evaluate(r).toValue(r,filePos);
				} else{
					return finalElse->evaluate(r).toValue(r,filePos);
				}
			}
			BasicBlock* StartBB = r.builder.GetInsertBlock();
			BasicBlock *ThenBB = r.CreateBlock("then",StartBB);
			BasicBlock *ElseBB = r.CreateBlock("else",StartBB);
			BasicBlock *MergeBB = r.CreateBlock("ifcont"/*,ThenBB,ElseBB*/);
			r.builder.CreateCondBr(cond, ThenBB, ElseBB);

			r.builder.SetInsertPoint(ThenBB);

			Value* ThenV = then->evaluate(r).castToV(r, returnType, filePos);

			r.builder.CreateBr(MergeBB);
			ThenBB = r.builder.GetInsertBlock();
			//r.addPred(MergeBB,ThenBB);
			r.builder.SetInsertPoint(ElseBB);

			Value* ElseV = finalElse->evaluate(r).castToV(r, returnType, filePos);

			r.builder.CreateBr(MergeBB);
			ElseBB = r.builder.GetInsertBlock();
			//r.addPred(MergeBB,ElseBB);

			r.builder.SetInsertPoint(MergeBB);
			PHINode *PN = r.builder.CreatePHI(returnType->getType(r), 2,"iftmp");
			PN->addIncoming(ThenV, ThenBB);
			PN->addIncoming(ElseV, ElseBB);
			return DATA::getConstant(PN, returnType);
		}

		Statement* simplify() override{
			return new TernaryOperator(filePos, condition->simplify(), then->simplify(), finalElse->simplify());
		}
		void write(ostream& a,String t) const override{
			a << condition << "?" << then << ":" << finalElse;
		}
};


#endif /* TERNARYOPERATOR_HPP_ */
