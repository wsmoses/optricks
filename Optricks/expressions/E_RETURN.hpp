/*
 * E_RETURN.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_RETURN_HPP_
#define E_RETURN_HPP_
#include "../constructs/Statement.hpp"
class E_RETURN : public Statement{
	public:
		Statement* inner;
		String name;
		JumpType jump;
		E_RETURN(PositionID id, Statement* t, String n, JumpType j) :
			Statement(id, voidClass), inner(t), name(n), jump(j){ };
		const Token getToken() const override{
			return T_RETURN;
		};

		FunctionProto* getFunctionProto() override final{
			error("Cannot getFunctionProto() for E_RETURN");
			return inner->getFunctionProto();
		}
		void setFunctionProto(FunctionProto* f) override final {
			error("Cannot setFunctionProto() for E_RETURN");
			inner->setFunctionProto(f); }
		ClassProto* getClassProto() override final{
			error("Cannot getClassProto() for E_RETURN");
			return inner->getClassProto(); }
		void setClassProto(ClassProto* f) override final {
			error("Cannot setClassProto() for E_RETURN");
			inner->setClassProto(f); }
		AllocaInst* getAlloc() override final{
			error("Cannot getAlloc() for E_RETURN");
			return inner->getAlloc(); };
		void setAlloc(AllocaInst* f) override final {
			error("Cannot setAlloc() for E_RETURN");
			inner->setAlloc(f); }
		String getObjName() override final {
			error("Cannot getObjName() for E_RETURN");
			return inner->getObjName(); }
		void setResolve(Value* v) override final {
			error("Cannot setResolve() for E_RETURN");
			inner->setResolve(v); }
		Value* getResolve() override final {
			error("Cannot getResolve() for E_RETURN");
			return inner->getResolve(); }

		void registerClasses(RData& r) override final{
			if(inner!=NULL)
			inner->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			if(inner!=NULL)
			inner->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			if(inner!=NULL)
			inner->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			if(inner!=NULL)
			inner->resolvePointers();
		}
		Statement* simplify() override{
			return this;
		}
		Value* evaluate(RData& r) override {
			Value* t = NULL;
	//		BasicBlock *Parent = r.builder.GetInsertBlock();
	//		BasicBlock *RETB = BasicBlock::Create(getGlobalContext(), "ret");
	//		r.builder.SetInsertPoint(RETB);
			if(inner!=NULL && inner->getToken()!=T_VOID){
				t = inner->evaluate(r);
				if(t==NULL) error("Why is t null?");
			}
			auto toBreak = r.getBlock(name, jump, (inner==NULL)?voidClass:(inner->returnType), r.builder.GetInsertBlock(), t);
	//		auto toBreak = r.getBlock(name, jump, returnType, RETB, t);
			r.builder.CreateBr(toBreak);
			//if(jump==RETURN)
				r.guarenteedReturn = true;
			return NULL;
	//		return RETB;
			//if(returnType==voidClass) return r.builder.CreateRetVoid();
			//else return r.builder.CreateRet(t);
		}
		void write (ostream& f,String b="") const override{
			switch(jump){
				case RETURN:
					f  << "return";
					if(inner!= NULL && inner->getToken()!=T_VOID) f << " " << inner;
					return;
				case BREAK:
					f  << "break";
					if(name!= "" ) f << " " << name;
					return;
				case CONTINUE:
					f  << "continue";
					if(name!= "" ) f << " " << name;
					return;
			}
		}
		ClassProto* checkTypes() override final{
			if(inner!=NULL) inner->checkTypes();
		}
};


#endif /* E_RETURN_HPP_ */
