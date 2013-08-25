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
		int64_t value;
		oint(PositionID a,  int64_t i): oobject(a,intClass), value(i){}
		void write(ostream& f, String b) const override{
			f << value;
		}
		Value* evaluate(RData& a) override final{
			return ConstantInt::get(INTTYPE, value, true);
		}
};
#endif /* OINT_HPP_ */
