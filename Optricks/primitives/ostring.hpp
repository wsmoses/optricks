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
		obool* operator == (oobject* s) override{
			return new obool(stringClass==(s)->returnType && ((ostring*)s)->value.compare(value)==0);
		}
		ostring* operator + (oobject* a) override{
			return new ostring(value+(String)(*a));
		}
};

#endif /* OSTRING_HPP_ */
