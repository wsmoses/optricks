/*
 * TernaryOperator.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef TERNARYOPERATOR_HPP_
#define TERNARYOPERATOR_HPP_

#include "../constructs/Statement.hpp"
#include "../containers/settings.hpp"

class TernaryOperator : public Statement{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		TernaryOperator(PositionID a, Statement* cond, Statement* th, Statement* const stat) :
			Statement(a), condition(cond), then(th), finalElse(stat){
		}

		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(Value* v) override final { error("Cannot set resolve"); }
		Value* getResolve() override final { error("Cannot get resolve"); return NULL;}
		const Token getToken() const override {
			return T_TERNARY;
		}

		void registerClasses(RData& r) override final{
			condition->registerClasses(r);
			then->registerClasses(r);
			finalElse->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			condition->registerFunctionArgs(r);
			then->registerFunctionArgs(r);
			finalElse->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			condition->registerFunctionDefaultArgs();
			then->registerFunctionDefaultArgs();
			finalElse->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			condition->resolvePointers();
			then->resolvePointers();
			finalElse->resolvePointers();
		}
		ClassProto* checkTypes() override{
			if(condition->checkTypes()!=boolClass) error("Cannot have non-bool as condition for ternary "+condition->returnType->name);
			auto g = then->checkTypes();
			auto b = finalElse->checkTypes();
			if(g!= b) error("Need matching types for ternary operator "+g->name+" and "+ b->name);
			return returnType = g;
		}
		Value* evaluate(RData& r) override{
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock *ThenBB = BasicBlock::Create(r.lmod->getContext(), "then", TheFunction);
			BasicBlock *ElseBB = BasicBlock::Create(r.lmod->getContext(), "else");
			BasicBlock *MergeBB = BasicBlock::Create(r.lmod->getContext(), "ifcont");

			r.builder.CreateCondBr(condition->evaluate(r), ThenBB, ElseBB);

			// Emit then value.
			r.builder.SetInsertPoint(ThenBB);

			Value* ThenV = then->evaluate(r);

			r.builder.CreateBr(MergeBB);
			// Codegen of 'Then' can change the current block, update ThenBB for the PHI.
			ThenBB = r.builder.GetInsertBlock();

			// Emit else block.
			TheFunction->getBasicBlockList().push_back(ElseBB);
			r.builder.SetInsertPoint(ElseBB);

			Value* ElseV = finalElse->evaluate(r);

			r.builder.CreateBr(MergeBB);
			// Codegen of 'Else' can change the current block, update ElseBB for the PHI.
			ElseBB = r.builder.GetInsertBlock();

			// Emit merge block.
			TheFunction->getBasicBlockList().push_back(MergeBB);
			r.builder.SetInsertPoint(MergeBB);
			PHINode *PN = r.builder.CreatePHI(returnType->type, 2,"iftmp");
			PN->addIncoming(ThenV, ThenBB);
			PN->addIncoming(ElseV, ElseBB);
			return PN;
		}
		Statement* simplify() override{
			return new TernaryOperator(filePos, condition->simplify(), then->simplify(), finalElse->simplify());
		}
		void write(ostream& a,String t) const override{
			a << condition << "?" << then << ":" << finalElse;
		}
};


#endif /* TERNARYOPERATOR_HPP_ */
