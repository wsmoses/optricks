/*
 * RealClass.hpp
 *
 *  Created on: Jan 3, 2014
 *      Author: Billy
 */

#ifndef REALCLASS_HPP_
#define REALCLASS_HPP_
#include "../AbstractClass.hpp"
class RealClass: public AbstractClass{
public:
	RealClass(String nam, LayoutType lt,ClassType ct, Type* tp=NULL):AbstractClass(nullptr,nam, nullptr, lt, ct, true, tp){};
	virtual Constant* getZero(PositionID id, bool negative=false) const=0;
	virtual Constant* getOne(PositionID id) const=0;
	virtual Constant* getValue(PositionID id, mpz_t const c) const=0;
};



#endif /* REALCLASS_HPP_ */