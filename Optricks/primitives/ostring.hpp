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
		//	ConstantInt* data[value.length()];
		//	for(unsigned int i = 0; i<value.length(); ++i){
		//		data[0] = ConstantInt::get(CHARTYPE, value[0], false);
		//	}
			//llvm::ConstantDataArray::get
//			return {INTTYPE, PointerType::get(CHARTYPE, 0)}
//

		//	std::vector<unsigned char> data;
//			for(auto& a: value) data.push_back(a);
	//		auto temp = ConstantDataArray::get(a.lmod->getContext(), ArrayRef<unsigned char>(data));
		/*	auto point = LLVMBuildGEP(a.builder, temp, NULL, 0, "tempn");
					//GetElementPtrInst::Create(ConstantInt::get(INTTYPE, 0, true), ArrayRef<Value*>(temp));
			LLVMOpaqueValue* tmp[2];
			LLVMOpaqueValue** V = tmp;
			V[0] = reinterpret_cast<LLVMOpaqueValue*>(ConstantInt::get(INTTYPE, value.length(), true));
			V[1] = reinterpret_cast<LLVMOpaqueValue*>(point);
			return reinterpret_cast<Value*>(LLVMConstStruct(V, 2, false));
//			return LLVMConstStruct(V, 2, false);
//*/
			return a.builder.CreateGlobalStringPtr(value,"tmpstr");
//			return a.builder.CreateGlobalStringPtr(value);
//			error("Do not use strings");
	//		return NULL;

			//return ConstantStruct::get (STRINGTYPE,ArrayRef<Constant*>(V) );
			//return llvm::ConstantArray::get(ArrayType::get(CHARTYPE, value.length()), ArrayRef<Constant*>(data));
			//return ConstantArray::get(
			//		uint8_t
			//		ArrayType::get(llvmChar(a,context),value.length())
			//, ArrayRef<uint8_t>(value.c_str()));
		}
};

#endif /* OSTRING_HPP_ */
