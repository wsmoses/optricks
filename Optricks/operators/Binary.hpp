/*
 * Binary.hpp
 *
 *  Created on: Mar 7, 2014
 *      Author: Billy
 */

#ifndef BINARY_HPP_
#define BINARY_HPP_

#include "../language/class/AbstractClass.hpp"

inline const AbstractClass* getBinopReturnType(PositionID filePos, const AbstractClass* cc, const AbstractClass* dd, const String operation);

inline const Data* getBinop(RData& r, PositionID filePos, const Data* value, const Evaluatable* ev, const String operation);

#endif /* BINARY_HPP_ */
