/*
 * IfStatement.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef IFSTATEMENT_HPP_
#define IFSTATEMENT_HPP_

#include "./Statement.hpp"
#include "./Block.hpp"
#include "../containers/settings.hpp"

class IfStatement : public Statement{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		IfStatement(PositionID a, Statement* cond, Statement* th, Statement* stat=VOID) :
			Statement(a, voidClass), condition(cond), then(th), finalElse(stat){
		}
		const Token getToken() const override {
			return T_IF;
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
			if(condition->checkTypes()!=boolClass) error("Cannot have non-bool as condition for if "+condition->returnType->name);
			then->checkTypes();
			finalElse->checkTypes();
			return returnType;
		}
		Value* evaluate(RData& r) override{
			bool ret = true;
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock* myStart = r.builder.GetInsertBlock();
			BasicBlock *ThenBB = BasicBlock::Create(r.lmod->getContext(), "then", TheFunction);
			BasicBlock *ElseBB = BasicBlock::Create(r.lmod->getContext(), "else", TheFunction);
			BasicBlock *MergeBB = BasicBlock::Create(r.lmod->getContext(), "ifcont", TheFunction);

			r.builder.CreateCondBr(condition->evaluate(r), ThenBB, ElseBB);

			r.guarenteedReturn = false;
			r.builder.SetInsertPoint(ThenBB);
			then->evaluate(r);
			if(!r.guarenteedReturn){
				r.builder.CreateBr(MergeBB);
			}
			ret = ret && r.guarenteedReturn;

			// Emit else block.
			//TheFunction->getBasicBlockList().push_back(ElseBB);
			r.builder.SetInsertPoint(ElseBB);

			r.guarenteedReturn = false;
			if(finalElse->getToken() != T_VOID){
				finalElse->evaluate(r);
				ret = ret && r.guarenteedReturn;
			}
			else ret = false;
			if(!r.guarenteedReturn){
				r.builder.CreateBr(MergeBB);
			}

			// Emit merge block.
//			TheFunction->getBasicBlockList().push_back(MergeBB);
			r.builder.SetInsertPoint(MergeBB);
			r.guarenteedReturn = ret;
			return MergeBB;
		}
		Statement* simplify() override{
			return new IfStatement(filePos, condition->simplify(), then->simplify(), finalElse->simplify());
		}
		void write(ostream& a,String t) const override{
			bool prevIf = false;
			if(t.length()>=2 && t[0]=='I' && t[1]=='F'){
				prevIf = true;
				t = t.substr(2);
			}
			a << "if ";
			if(finalElse->getToken()==T_IF && !prevIf) a << "  ";
			a << condition << " ";
			then->write(a,t);
			if(finalElse->getToken()!=T_VOID){
				a << ";" << endl;
				if(finalElse->getToken()==T_IF){
					a << t << "el";
					finalElse->write(a, "IF"+t);
				}
				else{
					a << t << "else ";
					finalElse->write(a, t);
				}
			}
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


#endif /* IFSTATEMENT_HPP_ */
