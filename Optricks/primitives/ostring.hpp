/*
 * ostring.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OSTRING_HPP_
#define OSTRING_HPP_

#include "oobjectproto.hpp"

class ostring : public oobject{
	public:
		String value;
		ostring(String i): oobject(stringClass), value(i){}
		operator String& (){
			return value;
		}
		operator String () const override{
			return value;
		}
		Value* evaluate(RData& a) override final{
			todo("Implement string evaluate");
			//return ConstantArray::get(
			//		uint8_t
			//		ArrayType::get(llvmChar(a,context),value.length())
			//, ArrayRef<uint8_t>(value.c_str()));
		}
};

#endif /* OSTRING_HPP_ */
