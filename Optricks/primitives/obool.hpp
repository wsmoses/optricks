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

		obool* operator == (oobject* s) override{
			return new obool( ( boolClass==(s)->returnType && ((obool*)s)->value==value));
		}
};


#endif /* OBOOL_HPP_ */
