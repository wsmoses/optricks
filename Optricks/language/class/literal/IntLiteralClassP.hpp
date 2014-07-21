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
		RealClass(nullptr,"intLiteral",LITERAL_LAYOUT,CLASS_INTLITERAL,BOOLTYPE)
		{
		LANG_M.addFunction(PositionID(0,0,"#intL"),"bitCount")->add(
						new BuiltinInlineFunction(new FunctionProto("bitCount",{AbstractDeclaration(this)},this),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
						return new IntLiteral(mpz_popcount(value));}), PositionID(0,0,"#float"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"trailingZeros")->add(
						new BuiltinInlineFunction(new FunctionProto("trailingZeros",{AbstractDeclaration(this)},this),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
						return new IntLiteral(mpz_scan1(value,0));}), PositionID(0,0,"#float"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"isNan")->add(
						new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"print")->add(
				new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
				char temp[mpz_sizeinbase (value, 10) + 2];
				mpz_get_str(temp, 10, value);
				auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
				//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
				for(const char* T = temp; *T !='\0'; ++T){
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, *T,false));
				}
				return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
			char temp[mpz_sizeinbase (value, 10) + 2];
			mpz_get_str(temp, 10, value);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			for(const char* T = temp; *T !='\0'; ++T){
				r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, *T,false));
			}
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, '\n',false));
			return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#int"),"chr")->add(
			new BuiltinInlineFunction(new FunctionProto("chr",{AbstractDeclaration(this)},&charClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			const auto& value = ((const IntLiteral*) args[0]->evaluate(r))->value;
			long int L;
			if(!mpz_fits_slong_p(value)){
				id.warning("Integer literal is too large for character");
				L = 0;}
			else
					L = mpz_get_si(value);
			if(L < 0 || L >= 1L << CHARTYPE->getIntegerBitWidth()){
				id.warning("Integer literal is too large for character");
				L = 0;
			}
			return new ConstantData(CharClass::getValue(L),&charClass);}), PositionID(0,0,"#float")
		);


		LANG_M.addFunction(PositionID(0,0,"#int"),"abs")->add(
					new BuiltinInlineFunction(new FunctionProto("abs",{AbstractDeclaration(this)},this),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					IntLiteral* out = new IntLiteral(0,0,0);
					mpz_abs(out->value, ((const IntLiteral*) args[0]->evaluate(r))->value);
					return out;
		}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#int"),"abs2")->add(
					new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},this),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					IntLiteral* out = new IntLiteral(0,0,0);
					mpz_mul(out->value, ((const IntLiteral*) args[0]->evaluate(r))->value, ((const IntLiteral*) args[0]->evaluate(r))->value);
					return out;
		}), PositionID(0,0,"#int"));
		///register methods such as print / tostring / tofile / etc
	}



#endif /* INTLITERALCLASSP_HPP_ */
