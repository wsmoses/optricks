/*
 * UnaryFunction.hpp
 *
 *  Created on: Mar 5, 2014
 *      Author: Billy
 */

#ifndef UNARYFUNCTION_HPP_
#define UNARYFUNCTION_HPP_

class UnaryFunction{
public:
	bool needsReference;
	const AbstractClass* returnType;
	std::function<const Data*(RData&, PositionID, const Data*)> toApply;
};




#endif /* UNARYFUNCTION_HPP_ */
