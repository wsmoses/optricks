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
		oobject* simplify() override final{
			return this;
		}

		ConstantFP* getConstant(RData& a) override final{
			return ConstantFP::get(a.lmod->getContext(), APFloat(value));
		}
		ConstantFP* evaluate(RData &a) override final{
			return ConstantFP::get(a.lmod->getContext(), APFloat(value));
		}
};

#endif /* ODOUBLE_HPP_ */
