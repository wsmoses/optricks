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
		obool* operator == (oobject* s) override;
};

#include "oint.hpp"
obool* odec::operator == (oobject* s) {
			return new obool(
							( intClass==((oobject*)s)->returnType && ((oint*)s)->value==value)
							||
							(decClass==((oobject*)s)->returnType && ((odec*)s)->value==value)
							);
}

#endif /* ODEC_HPP_ */
