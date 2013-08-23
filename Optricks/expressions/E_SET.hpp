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
		FunctionProto* getFunctionProto() override final{ return NULL; }
		const Token getToken() const final override{
			return T_SET;
		}
		ClassProto* checkTypes() final override{
			returnType = variable->checkTypes();
			value->checkTypes();
			if(value->returnType != variable->returnType )
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
		AllocaInst* getAlloc() override final{ return NULL; };
		Value* evaluate(RData& r) final override{
			Value* nex = value->evaluate(r);
			AllocaInst* aloc = variable->getAlloc();
			if(aloc==NULL) error("Cannot set variable of non-alloc");
			r.builder.CreateStore(nex, aloc);
			return nex;
		}
		E_SET* simplify() final override{
			return new E_SET(filePos, variable,(value->simplify()) );
		}
		void write(ostream& f, String s="") const final override{
			f << "SET(";
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
			f << ")";
		}
};

#endif /* E_SET_HPP_ */
