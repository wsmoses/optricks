/*
 * oint.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OINT_HPP_
#define OINT_HPP_

#include "oobjectproto.hpp"

class oint : public oobject{
	public:
		long long value;
		oint(PositionID a,  long long i): oobject(a,intClass), value(i){}
		operator String () const override final{
			std::stringstream convert;
			convert << value;
			return convert.str();
		}
		Value* evaluate(RData& a) override final{
			return ConstantInt::get(INTTYPE, value, true);
		}
};
#endif /* OINT_HPP_ */
