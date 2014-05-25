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
	RealClass(Scopable* s, String nam, LayoutType lt,ClassType ct, llvm::Type* tp=NULL):
		AbstractClass(s,nam, nullptr, lt, ct, true, tp){};
	virtual llvm::Constant* getZero(PositionID id, bool negative=false) const=0;
	virtual llvm::Constant* getOne(PositionID id) const=0;
	virtual llvm::Constant* getValue(PositionID id, const mpz_t& c) const=0;
};



#endif /* REALCLASS_HPP_ */
