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
		ostring(PositionID id, String i): oobject(id, stringClass), value(i){}
		operator String& (){
			return value;
		}
		operator String () const override{
			String t = "";
			for(auto& c: value){
				if(c=='\\') t+="\\\\";
				else if(c=='"') t+="\"";
				else t+=c;
			}
			return "\""+t+"\"";
		}
		Value* evaluate(RData& a) override final{
		//	ConstantInt* data[value.length()];
		//	for(unsigned int i = 0; i<value.length(); ++i){
		//		data[0] = ConstantInt::get(CHARTYPE, value[0], false);
		//	}
			//llvm::ConstantDataArray::get
			return ConstantDataArray::getString(a.lmod->getContext(), value);
			//return llvm::ConstantArray::get(ArrayType::get(CHARTYPE, value.length()), ArrayRef<Constant*>(data));
			//return ConstantArray::get(
			//		uint8_t
			//		ArrayType::get(llvmChar(a,context),value.length())
			//, ArrayRef<uint8_t>(value.c_str()));
		}
};

#endif /* OSTRING_HPP_ */
