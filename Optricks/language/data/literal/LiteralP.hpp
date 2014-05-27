/*
 * LiteralP.hpp
 *
 *  Created on: May 26, 2014
 *      Author: Billy
 */

#ifndef LITERALP_HPP_
#define LITERALP_HPP_
#include "./Literal.hpp"


const Data* Literal::callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const{
	id.error("Literal cannot be used as function "+str(type));
	return &VOID_DATA;
}
const AbstractClass* Literal::getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const{
	id.error("Literal cannot be used as function"+str(type));
	return &voidClass;
}
const AbstractClass* Literal::getMyClass(RData& r, PositionID id) const{
	id.error("Literal cannot be used as class"+str(type));
	return &voidClass;
}


#endif /* LITERALP_HPP_ */
