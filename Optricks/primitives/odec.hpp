/*
 * odouble.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef ODEC_HPP_
#define ODEC_HPP_

#include "oobjectproto.hpp"

#include "obool.hpp"

class odec : public oobject{
	public:
		double value;
		odec(double i): oobject(decClass), value(i){}
		operator double& (){
			return value;
		}
		operator String () const override{
			std::stringstream convert;
			convert << value;
			return convert.str();
		}
		Value* evaluate(RData &a) override final{
			return ConstantFP::get(a.lmod->getContext(), APFloat(value));
		}
};

#include "oint.hpp"

#endif /* ODEC_HPP_ */
