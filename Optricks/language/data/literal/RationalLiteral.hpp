/*
 * RationalLiteral.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef RATIONALLITERAL_HPP_
#define RATIONALLITERAL_HPP_
#include "../Data.hpp"
/*
//TODO allow for rational literals
class RationalLiteral:public Data{
public:
	mpq_class const value;
	RationalClass* const type;
	RationalLiteral(mpq_t const val, ClassProto* cp=NULL):value(val),type(cp){ assert(val);}
	AbstractClass* getReturnType() const override final;
	Value* getValue() const override final;
	const Data* castTo(RData& r, AbstractClass* right, PositionID id) const override final;
	Value* castToV(RData& r, AbstractClass* const right, const PositionID id) const override final;
};
*/
#endif /* RATIONALLITERAL_HPP_ */
