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
	RealClass(String nam, ClassType ct, Type* tp=NULL):AbstractClass(nullptr,nam, nullptr, PRIMITIVE_LAYOUT, ct, true, tp){};
	virtual Constant* getZero(bool negative=false) const=0;
	virtual Constant* getOne() const=0;
	virtual Constant* getValue(PositionID id, mpz_t const c) const=0;
	/*	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
};



#endif /* REALCLASS_HPP_ */
