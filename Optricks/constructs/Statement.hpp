/*
 * Statement.hpp
 *
 *  Created on: Jul 27, 2013
 *      Author: wmoses
 */

#ifndef STATEMENT_HPP_
#define STATEMENT_HPP_

#include "StatementProto.hpp"

ClassProto* VoidStatement::checkTypes(){
	return returnType = voidClass;
}

class Construct : public Statement{
public:
	virtual ~Construct(){};
	Construct(PositionID a, ClassProto* pr) : Statement(a, pr){};
	ReferenceElement* getMetadata() override final { error("Cannot get ReferenceElement of void"); return NULL; }
};
#endif /* STATEMENT_HPP_ */
