/*
 * odouble.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef ODOUBLE_HPP_
#define ODOUBLE_HPP_

#include "oobjectproto.hpp"
class odouble : public oobject{
	public:
		double value;
		odouble(PositionID a, double i): oobject(a, doubleClass), value(i){}

		void write(ostream& ss, String b) const override{
			ss << value;
		}
		DATA evaluate(RData &a) override final{
			return DATA::getConstant(getDouble(value),doubleClass);
		}
};

#endif /* ODOUBLE_HPP_ */
