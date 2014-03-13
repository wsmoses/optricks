/*
 * ClassLib.hpp
 *
 *  Created on: Mar 1, 2014
 *      Author: Billy
 */

#ifndef CLASSLIB_HPP_
#define CLASSLIB_HPP_

#include "./AbstractClass.hpp"
#include "./UserClass.hpp"
#include "./builtin/TupleClass.hpp"
#include "./builtin/IntClass.hpp"
#include "./builtin/FloatClass.hpp"
#include "./literal/IntLiteralClass.hpp"

const AbstractClass* getMin(const std::vector<const AbstractClass*>& ac, PositionID id){
	id.compilerError("GetMin inf has not been implemented");
	exit(1);
}
const AbstractClass* getMin(const AbstractClass* a, const AbstractClass* b, PositionID id){
	if(a==b) return a;
	id.compilerError("GetMin 2 has not been implemented");
	exit(1);
}
#endif /* CLASSLIB_HPP_ */
