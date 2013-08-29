/*
 * E_SET.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_SET_HPP_
#define E_SET_HPP_

#include "../constructs/Statement.hpp"
#include "./E_VAR.hpp"

class E_SET: public Statement{
	public:
		Statement* variable;
		Statement* value;
		E_SET(PositionID id, Statement* loc, Statement* e) : Statement(id){
			variable = loc;
			value = e;
		}
		const Token getToken() const final override{
			return T_SET;
		}
		ClassProto* checkTypes() final override{
			returnType = variable->checkTypes();
			value->checkTypes();
			if(!( variable->returnType == autoClass || value->returnType->hasCast(returnType)))
				error("E_SET of inconsistent types");
			return returnType;
		}

		void registerClasses(RData& r) override final{
			variable->registerClasses(r);
			if(value!=NULL) value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			variable->registerFunctionArgs(r);
			value->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			variable->registerFunctionDefaultArgs();
			if(value!=NULL) value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			variable->resolvePointers();
			if(value!=NULL) value->resolvePointers();
		};
		Value* evaluate(RData& r) final override{
			Value* nex = value->returnType->castTo(r, value->evaluate(r), variable->returnType);
			AllocaInst* aloc = variable->getAlloc();
			if(aloc==NULL) error("Cannot set variable of non-alloc");
			r.builder.CreateStore(nex, aloc);
			return nex;
		}
		E_SET* simplify() final override{
			return new E_SET(filePos, variable,(value->simplify()) );
		}
		void write(ostream& f, String s="") const final override{
			//f << "SET(";
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
			//f << ")";
		}

		FunctionProto* getFunctionProto() override final{ return value->getFunctionProto(); }
		void setFunctionProto(FunctionProto* f) override final { value->setFunctionProto(f); }
		ClassProto* getClassProto() override final{ return value->getClassProto(); }
		void setClassProto(ClassProto* f) override final { value->setClassProto(f); }
		AllocaInst* getAlloc() override final{ return value->getAlloc(); };
		void setAlloc(AllocaInst* f) override final { value->setAlloc(f); }
		String getObjName() override final { return value->getObjName(); }
		void setResolve(Value* v) override final { value->setResolve(v); }
		Value* getResolve() override final { error("Cannot get resolve"); return NULL;}
};

#endif /* E_SET_HPP_ */
