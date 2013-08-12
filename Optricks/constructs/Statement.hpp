/*
 * Statement.hpp
 *
 *  Created on: Jul 27, 2013
 *      Author: wmoses
 */

#ifndef STATEMENT_HPP_
#define STATEMENT_HPP_

#include "StatementProto.hpp"

#include "Expression.hpp" //needed to ensure jump.hpp is included
#include "Jump.hpp"

void* VoidStatement::evaluate(Jump& jump){
	jump = NOJUMP;
	return this;
}
Statement* VoidStatement::simplify(Jump& jump){
	jump = NOJUMP;
	return this;
}
const Token VoidStatement::getToken() const{
	return T_VOID;
}
bool VoidStatement::operator == (Statement* s) const{
	return getToken()==s->getToken();
}
void VoidStatement::write(ostream& a,String r) const{
	a << "void";
}

#endif /* STATEMENT_HPP_ */
