/*
 * Literal.hpp
 *
 *  Created on: Jan 4, 2014
 *      Author: Billy
 */

#ifndef LITERAL_HPP_
#define LITERAL_HPP_
#include "../Data.hpp"
#include "../../statement/Statement.hpp"
class Literal: public Data{
public:
	inline Literal(DataType r): Data(r){};
	inline const Literal* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const override;
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override;
	const AbstractClass* getMyClass(RData& r, PositionID id) const override;
};


#endif /* LITERAL_HPP_ */
