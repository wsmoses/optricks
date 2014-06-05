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
	if(toCast==this) return valueToCast;
	cerr << this << " " << toCast << endl << flush;
	id.compilerError("Casting array types has not been implemented "+toCast->getName());
	exit(1);
}


#endif /* ARRAYCLASSP_HPP_ */
