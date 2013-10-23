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
		virtual ~ostring(){};
		ostring(PositionID id, String i): oobject(id, c_stringClass), value(i){}
		void write(ostream& f, String b) const override{
			f<<"\"";
			for(auto& c: value){
				if(c=='\\')  f << "\\\\";
				else if(c=='"') f << "\"";
				else f << c;
			}
			f<<"\"";
		}
		oobject* simplify() override final{
			return this;
		}
		DATA evaluate(RData& a) override final{
			return DATA::getConstant(a.builder.CreateGlobalStringPtr(value,"tmpstr"),c_stringClass);//TODO maybe make location?
		}
};

#endif /* OSTRING_HPP_ */
