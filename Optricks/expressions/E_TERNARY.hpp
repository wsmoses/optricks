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
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
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
		void resolvePointers() override final{
			condition->resolvePointers();
			then->resolvePointers();
			finalElse->resolvePointers();
		}
		ClassProto* checkTypes(RData& r) override{
			if(condition->checkTypes(r)!=boolClass) error("Cannot have non-bool as condition for ternary "+condition->returnType->name);
			ClassProto* g = then->checkTypes(r);
			ClassProto* b = finalElse->checkTypes(r);
			ClassProto* tog = g->leastCommonAncestor(b);
			if(tog==NULL || tog==autoClass) error("Need matching types for ternary operator "+g->name+" and "+ b->name);
			return returnType = tog;
		}
		DATA evaluate(RData& r) override{

			//TODO support ((true)?a:b)++
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock *ThenBB = BasicBlock::Create(r.lmod->getContext(), "then", TheFunction);
			BasicBlock *ElseBB = BasicBlock::Create(r.lmod->getContext(), "else");
			BasicBlock *MergeBB = BasicBlock::Create(r.lmod->getContext(), "ifcont");

			r.builder.CreateCondBr(condition->evaluate(r).getValue(r), ThenBB, ElseBB);

			// Emit then value.
			r.builder.SetInsertPoint(ThenBB);

			Value* ThenV = then->evaluate(r).castTo(r, returnType, filePos).getValue(r);

			r.builder.CreateBr(MergeBB);
			// Codegen of 'Then' can change the current block, update ThenBB for the PHI.
			ThenBB = r.builder.GetInsertBlock();

			// Emit else block.
			TheFunction->getBasicBlockList().push_back(ElseBB);
			r.builder.SetInsertPoint(ElseBB);

			Value* ElseV = finalElse->evaluate(r).castTo(r, returnType, filePos).getValue(r);

			r.builder.CreateBr(MergeBB);
			// Codegen of 'Else' can change the current block, update ElseBB for the PHI.
			ElseBB = r.builder.GetInsertBlock();

			// Emit merge block.
			TheFunction->getBasicBlockList().push_back(MergeBB);
			r.builder.SetInsertPoint(MergeBB);
			PHINode *PN = r.builder.CreatePHI(returnType->getType(r), 2,"iftmp");
			PN->addIncoming(ThenV, ThenBB);
			PN->addIncoming(ElseV, ElseBB);
			return DATA::getConstant(PN, returnType);
		}

		Statement* simplify() override{
			return new TernaryOperator(filePos, condition->simplify(), then->simplify(), finalElse->simplify());
		}
		ClassProto* getSelfClass(RData& r) override final{ error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
		void write(ostream& a,String t) const override{
			a << condition << "?" << then << ":" << finalElse;
		}
};


#endif /* TERNARYOPERATOR_HPP_ */
