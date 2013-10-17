/*
 * obool.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OBOOL_HPP_
#define OBOOL_HPP_

#include "oobjectproto.hpp"

class obool: public oobject{
	public:
		bool value;
		obool(PositionID id, bool a) : oobject(id, boolClass) {
			value = a;
		}
		oobject* simplify() override final{
			return this;
		}
		void write(ostream& ss, String b) const override{
			ss << ((value)?("true"):("false"));
		}
		ConstantInt* getConstant(RData& r) override final{
			return ConstantInt::get(BOOLTYPE, value, false);
		}
		DATA evaluate(RData& a) override final{
			return DATA::getConstant(getBool(value));
		}
};


#endif /* OBOOL_HPP_ */
