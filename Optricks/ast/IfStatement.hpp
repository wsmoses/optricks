/*
 * IfStatement.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef IFSTATEMENT_HPP_
#define IFSTATEMENT_HPP_

#include "../language/statement/Statement.hpp"
#include "./Block.hpp"

class IfStatement : public ErrorStatement{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		virtual ~IfStatement(){};
		IfStatement(PositionID a, Statement* cond, Statement* th, Statement* stat=nullptr) :
			ErrorStatement(a), condition(cond), then(th), finalElse(stat){
			if(finalElse) assert(finalElse->getToken()!=T_VOID);
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			id.error("if-statement cannot act as function");
			exit(1);
		}
		bool hasCastValue(const AbstractClass* a) const override final{
			return false;
		}
		const Token getToken() const override {
			return T_IF;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe){
			then->collectReturns(vals,toBe);
			if(finalElse) finalElse->collectReturns(vals,toBe);
		}
		void registerClasses() const override final{
			condition->registerClasses();
			then->registerClasses();
			if(finalElse) finalElse->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			condition->registerFunctionPrototype(r);
			then->registerFunctionPrototype(r);
			if(finalElse) finalElse->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			condition->buildFunction(r);
			then->buildFunction(r);
			if(finalElse) finalElse->buildFunction(r);
		}
		const AbstractClass* getReturnType() const override{
			error("Cannot getREturnType of IF");
			exit(1);
		}
		const Data* evaluate(RData& r) const override{
			Value* cond = condition->evaluate(r)->castToV(r,boolClass,filePos);
			if(ConstantInt* c = dyn_cast<ConstantInt>(cond)){
				if(c->isOne()){
					then->evaluate(r);
				} else if(finalElse){
					finalElse->evaluate(r);
				}
				return VOID_DATA;
			}
			bool ret = true;
			BasicBlock* StartBB = r.builder.GetInsertBlock();
			BasicBlock *ThenBB = r.CreateBlock("then",StartBB);
			BasicBlock *ElseBB;
			BasicBlock *MergeBB;
			if(finalElse){
				ElseBB = r.CreateBlock("else",StartBB);
				MergeBB = r.CreateBlock("ifcont"/*,ThenBB,ElseBB*/);
				r.builder.CreateCondBr(cond, ThenBB, ElseBB);
			}
			else{
				ElseBB = nullptr;
				MergeBB = r.CreateBlock("ifcont"/*,StartBB,ThenBB*/);
				r.builder.CreateCondBr(cond, ThenBB, MergeBB);
			}
			//r.guarenteedReturn = false;
			r.builder.SetInsertPoint(ThenBB);

			then->evaluate(r);
			if(!r.hadBreak()){
				ThenBB = r.builder.GetInsertBlock();
				//r.addPred(MergeBB,ThenBB);
				r.builder.CreateBr(MergeBB);
			}
			//ret = ret && r.guarenteedReturn;

			// Emit else block.
			if(ElseBB){
				r.builder.SetInsertPoint(ElseBB);
				//r.guarenteedReturn = false;
				finalElse->evaluate(r);
				//ret = ret && r.guarenteedReturn;
				if(!r.hadBreak()){
					ElseBB = r.builder.GetInsertBlock();
					//r.addPred(MergeBB,ElseBB);
					r.builder.CreateBr(MergeBB);
				}
			}
			//else{
			//	ret = false;
			//}

			if(!ret){
				r.builder.SetInsertPoint(MergeBB);
			} else{
				r.DeleteBlock(MergeBB);
			}
			//r.guarenteedReturn = ret;
			return VOID_DATA;
		}
};


#endif /* IFSTATEMENT_HPP_ */