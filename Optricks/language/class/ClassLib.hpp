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

}
const AbstractClass* getMin(const AbstractClass* a, const AbstractClass* b, PositionID id){

}
#endif /* CLASSLIB_HPP_ */
