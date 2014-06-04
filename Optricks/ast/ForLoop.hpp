/*
 * ForLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */


#ifndef FORLOOP_HPPO_
#define FORLOOP_HPPO_

#include "../language/statement/Statement.hpp"
#include "./ForEachLoop.hpp"
class ForLoop : public ErrorStatement{
	public:
		mutable OModule module;
		Statement* initialize;
		Statement* condition;
		Statement* increment;
		Statement* toLoop;
		String name;
		virtual ~ForLoop(){};
		ForLoop(PositionID a, OModule* supMod, String n=""):ErrorStatement(a),module(supMod),name(n){
			increment = condition = initialize = toLoop = nullptr;//todo remove this
		}

		const AbstractClass* getMyClass(RData& r, PositionID id, const std::vector<TemplateArg>& args)const{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			return &voidClass;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
			toLoop->collectReturns(vals,toBe);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			id.error("for-loop cannot act as function");
			exit(1);
		}
		const Token getToken() const override {
			return T_FOR;
		}
		const Data* evaluate(RData& r) const override{
			if(initialize!=NULL && initialize->getToken()!= T_VOID) initialize->evaluate(r);
			llvm::BasicBlock* loopBlock;
			llvm::BasicBlock* incBlock;
			llvm::BasicBlock* afterBlock;

			llvm::Value* Cond1 = condition->evaluate(r)->castToV(r,&boolClass,filePos);
			//BasicBlock* StartBB = r.builder.GetInsertBlock();
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(Cond1)){
				if(c->isOne()){
					loopBlock = r.CreateBlock("loop");
					afterBlock = r.CreateBlock("endLoop");
					r.builder.CreateBr(loopBlock);
				}
				else{
					return &VOID_DATA;
				}
			} else{
				loopBlock = r.CreateBlock("loop");
				afterBlock = r.CreateBlock("endLoop");
				r.builder.CreateCondBr(Cond1, loopBlock, afterBlock);
			}
			incBlock = r.CreateBlock("inc");
			r.builder.SetInsertPoint(loopBlock);
			assert(incBlock); assert(afterBlock);
			Jumpable j(name, LOOP, nullptr, incBlock, afterBlock, NULL);
			r.addJump(&j);
			toLoop->evaluate(r);
			auto tmp = r.popJump();
			assert(tmp== &j);
			r.builder.CreateBr(incBlock);

			r.builder.SetInsertPoint(incBlock);
			if(increment!=NULL && increment->getToken()!= T_VOID) increment->evaluate(r);
			llvm::Value* EndCond = condition->evaluate(r)->castToV(r,&boolClass,filePos);
			if(!r.hadBreak()){
				if(auto c = llvm::dyn_cast<llvm::ConstantInt>(EndCond)){
					if(c->isOne()) r.builder.CreateBr(loopBlock);
					else r.builder.CreateBr(afterBlock);
				} else {
					r.builder.CreateCondBr(EndCond, loopBlock, afterBlock);
				}
			}

			r.builder.SetInsertPoint(afterBlock);
			for(const auto& dat: module.vars){
				decrementCount(r, filePos, dat);
			}
			return &VOID_DATA;
		}
		void registerClasses() const override final{
			condition->registerClasses();
			initialize->registerClasses();
			increment->registerClasses();
			toLoop->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			condition->registerFunctionPrototype(r);
			initialize->registerFunctionPrototype(r);
			increment->registerFunctionPrototype(r);
			toLoop->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			condition->buildFunction(r);
			initialize->buildFunction(r);
			increment->buildFunction(r);
			toLoop->buildFunction(r);
		}
};
#endif /* FORLOOP_HPP_ */
