/*
 * E_RETURN.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_RETURN_HPP_
#define E_RETURN_HPP_
#include "../language/statement/Statement.hpp"
#include "../language/class/builtin/VoidClass.hpp"
class E_RETURN : public ErrorStatement{
	public:
		virtual ~E_RETURN(){};
		Statement* inner;
		String name;
		JumpType jump;
		E_RETURN(PositionID id, Statement* t, String n, JumpType j) :
			ErrorStatement(id), inner(t), name(n), jump(j){ };
		const Token getToken() const override{
			return T_RETURN;
		};
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe){
			const AbstractClass* const n = (inner)?((const AbstractClass*) &voidClass):(inner->getReturnType());
			if(!toBe || toBe->classType==CLASS_AUTO) vals.push_back(n);
			else if(!n->hasCast(toBe)) error("Could not cast type "+n->getName() +" to "+toBe->getName());
		}
		void registerClasses() const override final{
			if(inner!=NULL)
			inner->registerClasses();
		}
		void registerFunctionPrototype(RData& r) const override final{
			if(inner!=NULL) inner->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) const override final{
			if(inner!=NULL) inner->buildFunction(r);
		}
		const Data* evaluate(RData& r) const override {
			const Data* t = (inner && inner->getToken()!=T_VOID)?(inner->evaluate(r)):(VOID_DATA);
			if(jump==YIELD){
				BasicBlock *RESUME = r.CreateBlock("postReturn",r.builder.GetInsertBlock());
				//BasicBlock* toBreak =
				r.getBlock(name, YIELD, r.builder.GetInsertBlock(), t, filePos, std::pair<BasicBlock*,BasicBlock*>(r.builder.GetInsertBlock(),RESUME));
				//assert(toBreak!=NULL);
//				r.addPred(toBreak,r.builder.GetInsertBlock());
				r.builder.SetInsertPoint(RESUME);
			} else if(jump==RETURN){
				if(t->type==R_VOID
						//t.getReturnType(ar, filePos)==voidClass
						//TODO check this
						//t.getType()==R_UNDEF
						)
					r.builder.CreateRetVoid();
				else{
					assert(r.functionReturn);
					Value* V = t->castToV(r, r.functionReturn, filePos);
					r.builder.CreateRet(V);
				}
			} else {
				BasicBlock* toBreak = r.getBlock(name, jump, r.builder.GetInsertBlock(), t, filePos, std::pair<BasicBlock*,BasicBlock*>(r.builder.GetInsertBlock(),NULL));
				if(toBreak!=NULL) r.builder.CreateBr(toBreak);
			}
			return VOID_DATA;
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			id.error("Return statement cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override final{
			error("Cannot getReturnType of E_RETURN");
			exit(1);
		}
};

#endif /* E_RETURN_HPP_ */
