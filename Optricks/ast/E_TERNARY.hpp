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
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
			const AbstractClass* const g = then->getFunctionReturnType(id,args);
			const AbstractClass* const b = finalElse->getFunctionReturnType(id,args);
			const AbstractClass* tog = g->leastCommonAncestor(b,filePos);
			if(tog==NULL || tog->classType==CLASS_AUTO) error("Need matching types for ternary operator "+g->getName()+" and "+ b->getName());
			assert(tog);
			return tog;
		}
		const AbstractClass* getReturnType() const override{
			const AbstractClass* const g = then->getReturnType();
			const AbstractClass* const b = finalElse->getReturnType();
			const AbstractClass* tog = g->leastCommonAncestor(b,filePos);
			if(tog==NULL || tog->classType==CLASS_AUTO) error("Need matching types for ternary operator "+g->getName()+" and "+ b->getName());
			assert(tog);
			return tog;
		}
		const Data* evaluate(RData& r) const override{
			Value* cond = condition->evaluate(r)->castToV(r,boolClass,filePos);
			if(ConstantInt* c = dyn_cast<ConstantInt>(cond)){
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
			BasicBlock* StartBB = r.builder.GetInsertBlock();
			BasicBlock *ThenBB = r.CreateBlock("then",StartBB);
			BasicBlock *ElseBB = r.CreateBlock("else",StartBB);
			BasicBlock *MergeBB = r.CreateBlock("ifcont"/*,ThenBB,ElseBB*/);
			BranchInst* branch = r.builder.CreateCondBr(cond, ThenBB, ElseBB);

			r.builder.SetInsertPoint(ThenBB);
			//todo allow castToPointer as well as value
			Value* ThenV = then->evaluate(r)->castToV(r, returnType, filePos);
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

			Value* ElseV = finalElse->evaluate(r)->castToV(r, returnType, filePos);
			bool isSingleInstruction2 = ElseBB->getInstList().size()==0;
			if(isSingleInstruction2){
				if(isSingleInstruction){
					branch->dropAllReferences();
					r.DeleteBlock(ElseBB);
					r.builder.SetInsertPoint(StartBB);
					return new ConstantData(r.builder.CreateSelect(cond, ThenV,ElseV), returnType);
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
			PHINode *PN = r.builder.CreatePHI(returnType->type, 2,"iftmp");
			PN->addIncoming(ThenV, ThenBB);
			PN->addIncoming(ElseV, ElseBB);
			return new ConstantData(PN, returnType);
		}

		Statement* simplify() override{
			return new TernaryOperator(filePos, condition->simplify(), then->simplify(), finalElse->simplify());
		}
		void write(ostream& a,String t) const override{
			a << condition << "?" << then << ":" << finalElse;
		}
};


#endif /* TERNARYOPERATOR_HPP_ */
