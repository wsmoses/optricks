/*
 * DoWhileLoop.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef DOWHILELOOP_HPPO_
#define DOWHILELOOP_HPPO_

#include "../language/statement/Statement.hpp"
#include "../language/data/Data.hpp"

class DoWhileLoop : public ErrorStatement{
public:
	Statement* const condition;
	Statement* const statement;
	String name;
	virtual ~DoWhileLoop(){};

	const AbstractClass* getMyClass(RData& r, PositionID id, const std::vector<TemplateArg>& args)const{
		id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
	}
	DoWhileLoop(PositionID a, Statement * cond, Statement* stat,String n="") :
		ErrorStatement(a),
		condition(cond), statement(stat){
		name = n;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		id.error("Do-while cannot act as function");
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
		return T_DOWHILE;
	}
	const Data* evaluate(RData& r) const override{
		llvm::BasicBlock* loopBlock = r.CreateBlock("loop");
		llvm::BasicBlock* incBlock = r.CreateBlock("inc");
		llvm::BasicBlock* afterBlock = r.CreateBlock("afterloop");
		r.builder.CreateBr(loopBlock);
		r.builder.SetInsertPoint(loopBlock);
		assert(incBlock); assert(afterBlock);
		Jumpable j(name, LOOP, nullptr, incBlock, afterBlock, NULL);
		r.addJump(&j);
		statement->evaluate(r);
#ifndef NDEBUG
		auto tmp = r.popJump();
		assert(tmp== &j);
#else
		r.popJump();
#endif

		r.builder.CreateBr(incBlock);
		r.builder.SetInsertPoint(incBlock);
		llvm::Value *EndCond = condition->evaluate(r)->castToV(r,&boolClass,filePos);
		if(!r.hadBreak()){
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(EndCond)){
				if(c->isOne()) r.builder.CreateBr(loopBlock);
				else r.builder.CreateBr(afterBlock);
			} else {
				r.builder.CreateCondBr(EndCond, loopBlock, afterBlock);
			}
		}
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
#endif /* DOWHILELOOP_HPP_ */

