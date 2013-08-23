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
		E_RETURN(PositionID id, Statement* t) : Statement(id, t->returnType), inner(t) { };
		const Token getToken() const override{
			return T_RETURN;
		};

		FunctionProto* getFunctionProto() override final{ return inner->getFunctionProto(); }
		void setFunctionProto(FunctionProto* f) override final { inner->setFunctionProto(f); }
		ClassProto* getClassProto() override final{ return inner->getClassProto(); }
		void setClassProto(ClassProto* f) override final { inner->setClassProto(f); }
		AllocaInst* getAlloc() override final{ return inner->getAlloc(); };
		void setAlloc(AllocaInst* f) override final { inner->setAlloc(f); }
		String getObjName() override final { return inner->getObjName(); }
		void setResolve(Value* v) override final { inner->setResolve(v); }
		Value* getResolve() override final { return inner->getResolve(); }

		void registerClasses(RData& r) override final{
			inner->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			inner->registerFunctionArgs(r);
		}
		void registerFunctionDefaultArgs() override final{
			inner->registerFunctionDefaultArgs();
		}
		void resolvePointers() override final{
			inner->resolvePointers();
		}
		Value* evaluate(RData& r) override {
			Value* t;
			if(inner->getToken()!=T_VOID) t = inner->evaluate(r);
			if(returnType==voidClass) return r.builder.CreateRetVoid();
			else return r.builder.CreateRet(t);
		}
		//TODO verify that this is valid and does not call order of ops again
		Statement* simplify() override{
			return inner->simplify();
		}
		void write (ostream& f,String b="") const override{
			f  << "(" << inner << ")";
		}
		ClassProto* checkTypes() override final{
			return returnType = inner->checkTypes();
		}
};


#endif /* E_RETURN_HPP_ */
