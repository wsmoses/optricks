/*
 * AbstractDeclaration.hpp
 *
 *  Created on: Jan 10, 2014
 *      Author: Billy
 */

#ifndef ABSTRACTDECLARATION_HPP_
#define ABSTRACTDECLARATION_HPP_
#include "includes.hpp"
struct AbstractDeclaration{
public:
	const AbstractClass* declarationType;
	String declarationVariable;
	Evaluatable* const defaultValue;
	inline AbstractDeclaration(const AbstractClass* ac, String nam="", Evaluatable* dv=nullptr):
		declarationType(ac),declarationVariable(nam),defaultValue(dv){
		assert(ac);
		//if(dv) assert(dv->type!=R_VOID);
	}
};


#endif /* ABSTRACTDECLARATION_HPP_ */
