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

#endif /* STATEMENT_HPP_ */
