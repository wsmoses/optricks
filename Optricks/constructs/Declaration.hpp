/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../constructs/Statement.hpp"
#include "../expressions/E_VAR.hpp"

#define DECLR_P_
class Declaration: public Statement{
	public:
	E_VAR* classV;
	E_VAR* variable;
	Statement* value;
	Declaration(PositionID id, E_VAR* v, E_VAR* loc, Statement* e=NULL) : Statement(id, voidClass){
		classV = v;
		variable = loc;
		value = e;
	}
	FunctionProto* getFunctionProto() override final{ return NULL; }
	const Token getToken() const final override{
		return T_DECLARATION;
	}
	ClassProto* checkTypes() final override{
		classV->checkTypes();
		if(classV->returnType!=classClass) error("Argument " + classV->pointer->name + "is not a class");
		ClassProto*& meta = variable->pointer->resolveReturnClass();
		if(meta==NULL) meta = classV->pointer->resolveSelfClass();
		else{
			cout << "Why is meta of declaration non-null? " << meta << endl << flush ;
		}
		variable->checkTypes();
		if(value!=NULL){
			value->checkTypes();
			if(value->returnType != classV->pointer->resolveSelfClass() )
				error("Declaration of inconsistent types");
		}
		return returnType;
	}

	void registerClasses(RData& r) override final{
		classV->registerClasses(r);
		variable->registerClasses(r);
		if(value!=NULL) value->registerClasses(r);
	}
	void registerFunctionArgs(RData& r) override final{
		classV->registerFunctionArgs(r);
		variable->registerFunctionArgs(r);
		if(value!=NULL) value->registerFunctionArgs(r);
		//TODO add name in table of args?
	};
	void registerFunctionDefaultArgs() override final{
		classV->registerFunctionDefaultArgs();
		variable->registerFunctionDefaultArgs();
		if(value!=NULL) value->registerFunctionDefaultArgs();
	};
	void resolvePointers() override final{
		classV->resolvePointers();
		variable->resolvePointers();
		if(value!=NULL) value->resolvePointers();
	};
	Value* evaluate(RData& r) final override{
		error("Todo: allow declaration evaluation");
	}
	Declaration* simplify() final override{
		return new Declaration(filePos, classV, variable, (value==NULL)?NULL:(value->simplify()) );
	}
	void write(ostream& f, String s="") const final override{
		f << "DECLARATION(";
		if(classV!=NULL) f << classV << " ";
		else f << "auto ";
		variable->write(f);
		if(value!=NULL) {
			f << "=";
			value->write(f);
		}
		f << ")";
	}
};


#endif /* Declaration_HPP_ */
