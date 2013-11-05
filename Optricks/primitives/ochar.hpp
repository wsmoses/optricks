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
			f<<"\"";
				if(value=='\\')  f << "\\\\";
				else if(value=='\t') f << "\\t";
				else if(value=='\r') f << "\\r";
				else if(value=='\n') f << "\\n";
				else if(value=='\v') f << "\\v";
				else if(value=='"') f << "\\\"";
				else if(value==0) f << "\\0";
				else f << value;
			f<<"\"";
		}
		DATA evaluate(RData& a) override final{
			return DATA::getConstant(ConstantInt::get(CHARTYPE, value, false),charClass);
		}
};
#endif /* OCHAR_HPP_ */
