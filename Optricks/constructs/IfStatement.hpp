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

class IfStatement : public Construct{
	public:
		Statement* condition;
		Statement* then;
		Statement* const finalElse;
		virtual ~IfStatement(){};
		IfStatement(PositionID a, Statement* cond, Statement* th, Statement* stat=VOID) :
			Construct(a, voidClass), condition(cond), then(th), finalElse(stat){
		}
		const Token getToken() const override {
			return T_IF;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals, ClassProto* toBe){
			then->collectReturns(r, vals,toBe);
			if(finalElse!=NULL) finalElse->collectReturns(r, vals,toBe);
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
			if(condition->checkTypes(r)!=boolClass) error("Cannot have non-bool as condition for if "+condition->returnType->name);
			then->checkTypes(r);
			finalElse->checkTypes(r);
			return returnType;
		}
		DATA evaluate(RData& r) override{
			Value* cond = condition->evaluate(r).getValue(r);
			if(ConstantInt* c = dyn_cast<ConstantInt>(cond)){
				if(c->isOne()){
					then->evaluate(r);
				} else if(finalElse!=NULL && finalElse->getToken()!=T_VOID){
					finalElse->evaluate(r);
				}
				return DATA::getNull();
			}
			bool ret = true;
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			BasicBlock *ThenBB = BasicBlock::Create(r.lmod->getContext(), "then", TheFunction);
			BasicBlock *ElseBB = BasicBlock::Create(r.lmod->getContext(), "else", TheFunction);
			BasicBlock *MergeBB = BasicBlock::Create(r.lmod->getContext(), "ifcont", TheFunction);
			if(finalElse->getToken()!=T_VOID)
				r.builder.CreateCondBr(cond, ThenBB, ElseBB);
			else
				r.builder.CreateCondBr(cond, ThenBB, MergeBB);
			r.guarenteedReturn = false;
			r.builder.SetInsertPoint(ThenBB);
			then->evaluate(r);
			if(!r.guarenteedReturn){
				r.builder.CreateBr(MergeBB);
			}
			ret = ret && r.guarenteedReturn;

			// Emit else block.
			if(finalElse->getToken() != T_VOID){
				r.builder.SetInsertPoint(ElseBB);
				r.guarenteedReturn = false;
				finalElse->evaluate(r);
				ret = ret && r.guarenteedReturn;
				if(!r.guarenteedReturn) r.builder.CreateBr(MergeBB);
			}
			else{
				ret = false;
				TheFunction->getBasicBlockList().remove(ElseBB);
			}

			if(!ret){
				r.builder.SetInsertPoint(MergeBB);
			} else{
				TheFunction->getBasicBlockList().remove(MergeBB);
			}

			// Emit merge block.
//
			r.guarenteedReturn = ret;
			return DATA::getNull();
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
};


#endif /* IFSTATEMENT_HPP_ */
