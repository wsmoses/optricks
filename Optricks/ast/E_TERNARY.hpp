/*
 * TernaryOperator.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef TERNARYOPERATOR_HPP_
#define TERNARYOPERATOR_HPP_

#include "../language/statement/Statement.hpp"

class TernaryOperator : public ErrorStatement{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		virtual ~TernaryOperator(){};
		TernaryOperator(PositionID a, Statement* cond, Statement* th, Statement* const stat) :
			ErrorStatement(a), condition(cond), then(th), finalElse(stat){
		}

		void reset() const override final{ condition->reset(); then->reset(); if(finalElse) finalElse->reset(); }
		const AbstractClass* getMyClass(PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
		const Token getToken() const override {
			return T_TERNARY;
		}

		void registerClasses() const override final{
			condition->registerClasses();
			then->registerClasses();
			finalElse->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			condition->registerFunctionPrototype(r);
			then->registerFunctionPrototype(r);
			finalElse->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			condition->buildFunction(r);
			then->buildFunction(r);
			finalElse->buildFunction(r);
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			const AbstractClass* const g = then->getFunctionReturnType(id,args, isClassMethod);
			const AbstractClass* const b = finalElse->getFunctionReturnType(id,args, isClassMethod);
			const AbstractClass* tog = getMin(g, b, filePos);
			if(tog==NULL) error("Need matching types for ternary operator "+g->getName()+" and "+ b->getName());
			assert(tog);
			return tog;
		}
		const AbstractClass* getReturnType() const override{
			const AbstractClass* const g = then->getReturnType();
			const AbstractClass* const b = finalElse->getReturnType();
			const AbstractClass* tog = getMin(g, b,filePos);
			if(tog==NULL) error("Need matching types for ternary operator "+g->getName()+" and "+ b->getName());
			assert(tog);
			return tog;
		}
		const Data* evaluate(RData& r) const override{
			auto cond = condition->evaluate(r)->castToV(r,&boolClass,filePos);
			assert(cond);
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(cond)){
				if(c->isOne()){
					return then->evaluate(r)->toValue(r,filePos);
				} else{
					return finalElse->evaluate(r)->toValue(r,filePos);
				}
			}
			const AbstractClass* const returnType = getReturnType();
			/*
			if(then->isConstant() && finalElse->isConstant()){
				Value* ThenV = then->evaluate(r)->castToV(r, returnType, filePos);
				Value* ElseV = finalElse->evaluate(r)->castToV(r, returnType, filePos);
				return r.builder.CreateSelect(cond, ThenV, ElseV);
			}*/
			llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
			llvm::BasicBlock *ThenBB = r.CreateBlock("then",StartBB);
			llvm::BasicBlock *ElseBB = r.CreateBlock("else",StartBB);
			llvm::BasicBlock *MergeBB = r.CreateBlock("ifcont"/*,ThenBB,ElseBB*/);
			llvm::BranchInst* branch = r.builder.CreateCondBr(cond, ThenBB, ElseBB);

			r.builder.SetInsertPoint(ThenBB);
			//todo allow castToPointer as well as value
			llvm::Value* ThenV = then->evaluate(r)->castToV(r, returnType, filePos);
			assert(ThenV);
			bool isSingleInstruction = ThenBB->getInstList().size()==0;
			if(isSingleInstruction){
				branch->setSuccessor(0,MergeBB);
				r.DeleteBlock(ThenBB);
				ThenBB = StartBB;
			} else {
				r.builder.CreateBr(MergeBB);
				ThenBB = r.builder.GetInsertBlock();
			}
			//r.addPred(MergeBB,ThenBB);
			r.builder.SetInsertPoint(ElseBB);

			llvm::Value* ElseV = finalElse->evaluate(r)->castToV(r, returnType, filePos);
			assert(ElseV);
			bool isSingleInstruction2 = ElseBB->getInstList().size()==0;
			if(isSingleInstruction2){
				if(isSingleInstruction){
					branch->eraseFromParent();
					r.DeleteBlock(ElseBB);
					r.builder.SetInsertPoint(StartBB);
					auto S = r.builder.CreateSelect(cond, ThenV,ElseV);
					return new ConstantData(S , returnType);
				} else {
					branch->setSuccessor(1,MergeBB);
					r.DeleteBlock(ElseBB);
					ElseBB = StartBB;
				}
			} else {
				r.builder.CreateBr(MergeBB);
				ElseBB = r.builder.GetInsertBlock();
			}

			r.builder.SetInsertPoint(MergeBB);
			llvm::PHINode *PN = r.builder.CreatePHI(returnType->type, 2,"iftmp");
			PN->addIncoming(ThenV, ThenBB);
			PN->addIncoming(ElseV, ElseBB);
			return new ConstantData(PN, returnType);
		}
};


#endif /* TERNARYOPERATOR_HPP_ */
