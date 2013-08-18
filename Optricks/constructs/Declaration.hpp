/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../constructs/Expression.hpp"
#include "../expressions/E_VAR.hpp"
class Declaration: public Statement{
	public:
	E_VAR* classV;
	E_VAR* variable;
	Expression* value;
	Declaration(E_VAR* v, E_VAR* loc, Expression* e=NULL){
		classV = v;
		variable = loc;
		value = e;
	}
	const Token getToken() const final override{
		return T_DECLARATION;
	}
	void checkTypes() final override{
		classV->checkTypes();
		variable->checkTypes();
		if(value!=NULL){
			value->checkTypes();
			if(value->returnType != dynamic_cast<oclass*>(classV->pointer->resolveMeta()))
				todo("Declaration of inconsistent types");
		}
	}
	Value* evaluate(RData& r, LLVMContext& c) final override{
		todo("Todo: allow declaration evaluation");
	}
	Statement* simplify(Jump& j) final override{
		if(value!=NULL)
		return new Declaration(classV, variable, value->simplify());
		else return this;
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
