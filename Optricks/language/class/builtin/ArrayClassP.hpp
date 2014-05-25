/*
 * ArrayClassP.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef ARRAYCLASSP_HPP_
#define ARRAYCLASSP_HPP_

#include "./ArrayClass.hpp"

llvm::Value* ArrayClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
	id.compilerError("Casting array types has not been implemented");
	exit(1);
}


#endif /* ARRAYCLASSP_HPP_ */
