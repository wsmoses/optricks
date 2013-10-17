/*
 * ochar.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OCHAR_HPP_
#define OCHAR_HPP_

#include "oobjectproto.hpp"

class ochar : public oobject{
	public:
		char value;
		ochar(PositionID a,  char i): oobject(a,charClass), value(i){}
		void write(ostream& f, String b) const override{
			f << value;
		}
		oobject* simplify() override final{
			return this;
		}
		ConstantInt* getConstant(RData& r) override final{
			return ConstantInt::get(CHARTYPE, value, false);
		}
		DATA evaluate(RData& a) override final{
			return DATA::getConstant(ConstantInt::get(CHARTYPE, value, false));
		}
};
#endif /* OCHAR_HPP_ */
