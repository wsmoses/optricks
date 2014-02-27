/*
 * EvaluatableP.hpp
 *
 *  Created on: Feb 13, 2014
 *      Author: Billy
 */

#ifndef EVALUATABLEP_HPP_
#define EVALUATABLEP_HPP_
#include "Evaluatable.hpp"
#include "../class/AbstractClass.hpp"

llvm::Value* Evaluatable::evalV(RData& r,PositionID id) const{
		return evaluate(r)->getValue(r,id);
}

#endif /* EVALUATABLEP_HPP_ */
