/*
 * WhileLoop.hpp
 *
 *  Created on: Jan 30, 2014
 *      Author: Billy
 */

#ifndef WHILELOOP_HPP_
#define WHILELOOP_HPP_


#include "../language/statement/Statement.hpp"
#include "../language/data/Data.hpp"

class WhileLoop : public ErrorStatement{
	public:
		Statement* const condition;
		Statement* const statement;
		String name;
		virtual ~WhileLoop(){};
		WhileLoop(PositionID a, Statement * cond, Statement* stat,String n="") :
			ErrorStatement(a),
			condition(cond), statement(stat){
			name = n;
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			id.error("While-loop cannot act as function");
			exit(1);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
			statement->collectReturns(vals, toBe);
		}
		//bool hasCastValue(AbstractClass* a) override final{
		//	return false;
		//}
		const AbstractClass* getReturnType() const override final{
			return &voidClass;
		}
		const Token getToken() const override {
			return T_WHILE;
		}
		const Data* evaluate(RData& r) const override{
			BasicBlock* prev = r.builder.GetInsertBlock();
			BasicBlock *incBlock = r.CreateBlock("inc");
			auto br = r.builder.CreateBr(incBlock);
			r.builder.SetInsertPoint(incBlock);
			Value* v = condition->evalCastV(r,&boolClass,filePos);
			if(ConstantInt* c = dyn_cast<ConstantInt>(v)){
				if(c->isOne()){
					//is always true
					BasicBlock *afterBlock = r.CreateBlock("afterloop");
					Jumpable j(name, LOOP, incBlock, afterBlock, NULL);
					r.addJump(&j);
					statement->evaluate(r);
					#ifndef NDEBUG
						auto tmp = r.popJump();
						assert(tmp== &j);
					#else
						r.popJump();
					#endif

					r.builder.CreateBr(incBlock);
					r.builder.SetInsertPoint(afterBlock);
					return &VOID_DATA;
				} else {
					//is always false
					br->removeFromParent();
					r.builder.SetInsertPoint(prev);
					r.DeleteBlock(incBlock);
					return &VOID_DATA;
				}
			}
			BasicBlock *loopBlock = r.CreateBlock("loop");
			BasicBlock *afterBlock = r.CreateBlock("afterloop");
			r.builder.CreateCondBr(v, loopBlock, afterBlock);
			r.builder.SetInsertPoint(loopBlock);
			Jumpable j(name, LOOP, incBlock, afterBlock, NULL);
			r.addJump(&j);
			statement->evaluate(r);
			#ifndef NDEBUG
				auto tmp = r.popJump();
				assert(tmp== &j);
			#else
				r.popJump();
			#endif
			if(!r.hadBreak())
				r.builder.CreateBr(incBlock);
			r.builder.SetInsertPoint(afterBlock);
			return &VOID_DATA;
		}
		void registerClasses() const override final{
			condition->registerClasses();
			statement->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			condition->registerFunctionPrototype(r);
			statement->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			condition->buildFunction(r);
			statement->buildFunction(r);
		}
};



#endif /* WHILELOOP_HPP_ */
