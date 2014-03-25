/*
 * IntLiteralClassP.hpp
 *
 *  Created on: Mar 24, 2014
 *      Author: Billy
 */

#ifndef INTLITERALCLASSP_HPP_
#define INTLITERALCLASSP_HPP_

#include "./IntLiteralClass.hpp"
#include "../../data/literal/IntLiteral.hpp"
	IntLiteralClass::IntLiteralClass(bool b):
		RealClass("intLiteral",LITERAL_LAYOUT,CLASS_INTLITERAL,llvm::IntegerType::get(getGlobalContext(), 1))
		{

		LANG_M->addFunction(PositionID(0,0,"#intL"),"isNan")->add(
						new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
						nullptr,[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
						assert(args.size()==1);
						return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));
		LANG_M->addFunction(PositionID(0,0,"#intL"),"print")->add(
				new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
				nullptr,[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
				char temp[mpz_sizeinbase (value, 10) + 2];
				mpz_get_str(temp, 10, value);
				auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
				//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
				for(const char* T = temp; *T !='\0'; ++T){
					r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
				}
				return VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M->addFunction(PositionID(0,0,"#intL"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			nullptr,[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
			char temp[mpz_sizeinbase (value, 10) + 2];
			mpz_get_str(temp, 10, value);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			for(const char* T = temp; *T !='\0'; ++T){
				r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
			}
			r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, '\n',false));
			return VOID_DATA;}), PositionID(0,0,"#int"));
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}



#endif /* INTLITERALCLASSP_HPP_ */
