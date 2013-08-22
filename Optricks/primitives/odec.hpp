/*
 * odouble.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef ODEC_HPP_
#define ODEC_HPP_

#include "oobjectproto.hpp"
class odec : public oobject{
	public:
		double value;
		odec(PositionID a, double i): oobject(a, decClass), value(i){}
		operator String () const override final{
			std::stringstream convert;
			convert << value;
			return convert.str();
		}
		Value* evaluate(RData &a) override final{
			return ConstantFP::get(a.lmod->getContext(), APFloat(value));
		}
};

#endif /* ODEC_HPP_ */
