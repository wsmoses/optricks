/*
 * obool.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OBOOL_HPP_
#define OBOOL_HPP_

#include "oobjectproto.hpp"

#define OCLASS_P_
class obool: public oobject{
	public:
		bool value;
		explicit obool(bool a) : oobject(boolClass) { value = a; }
		operator bool& () { return value; }
		operator String () const override{
			return value?"true":"false";
		}
		Value* evaluate(RData& a,LLVMContext& context) override final{
			return ConstantInt::get(IntegerType::get(context, 1), value, false);
		}
};


#endif /* OBOOL_HPP_ */
