/*
 * oint.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OINT_HPP_
#define OINT_HPP_

#include "oobjectproto.hpp"
#include <string>
class oint : public oobject{
	public:
		int value;
		oint(int i): oobject(intClass), value(i){}
		operator int& (){
			return value;
		}
		operator int () {
			return value;
		}
		operator String () const override{
			std::stringstream convert;
			convert << value;
			return convert.str();
		}
		oobject* operator + (oobject* a) override;
		obool* operator == (oobject* s) override;
};


#include "odec.hpp"
#include "ostring.hpp"

oobject* oint::operator + (oobject* a){
	if(a->returnType== intClass)
				return new oint(value+((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(value+((odec*)a)->value);
	if(a->returnType==  stringClass)
				return new ostring(
							(String)(*this)
						+((ostring*)a)->value);

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);

}
obool* oint::operator == (oobject* s){
			return new obool( ( intClass==(s)->returnType && ((oint*)s)->value==value)
							|| (decClass==(s)->returnType && ((odec*)s)->value==value));
}

#endif /* OINT_HPP_ */
