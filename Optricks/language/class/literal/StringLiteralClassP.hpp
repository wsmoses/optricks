/*
 * StringLiteralClassP.hpp
 *
 *  Created on: Mar 25, 2014
 *      Author: Billy
 */

#ifndef STRINGLITERALCLASSP_HPP_
#define STRINGLITERALCLASSP_HPP_


#include "../builtin/CStringClass.hpp"
#include "./StringLiteralClass.hpp"
#include "../../data/literal/StringLiteral.hpp"
	StringLiteralClass::StringLiteralClass(bool b):
		AbstractClass(nullptr,"stringLiteral",nullptr, LITERAL_LAYOUT,CLASS_STRLITERAL,true,BOOLTYPE)
		{

		LANG_M.addFunction(PositionID(0,0,"#stringL"),"print")->add(
				new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
				auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
				//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
				for(const auto& a: value){
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
				}
				return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",std::vector<AbstractDeclaration>(),&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==0);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, '\n',false));
			return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#intL"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			for(const auto& a:value){
				r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
			}
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, '\n',false));
			return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#str"),"printf")->add(
				new BuiltinInlineFunction(
						new FunctionProto("printf",{AbstractDeclaration(this)},&intClass,true),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()>=1);
				//TODO custom formatting for printf (and checks for literals / correct format / etc)
				const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
				llvm::SmallVector<llvm::Type*,1> t_args(1);
				t_args[0] = C_STRINGTYPE;
				auto CU = r.getExtern("printf", llvm::FunctionType::get(c_intClass.type, t_args,true));
				llvm::SmallVector<llvm::Value*,1> m_args(args.size());
				m_args[0] = r.getConstantCString(value);
				assert(m_args[0]);
				for(unsigned i=1; i<args.size(); i++){
					m_args[i] = args[i]->evalV(r, id);
					assert(m_args[i]);
				}
				llvm::Value* V = r.builder.CreateCall(CU, m_args);
				V = r.builder.CreateSExtOrTrunc(V, intClass.type);
				return new ConstantData(V, &intClass);
			}), PositionID(0,0,"#int"));


		LANG_M.addFunction(PositionID(0,0,"#str"),"sprintf")->add(
				new BuiltinInlineFunction(
						new FunctionProto("sprintf",{AbstractDeclaration(&c_stringClass),AbstractDeclaration(this)},&intClass,true),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()>=2);
				//TODO custom formatting for printf (and checks for literals / correct format / etc)
				assert(args[1]);
				auto TP = args[1]->evaluate(r);
				assert(TP);
				assert(TP->type==R_STR);
				const auto& value = ((const StringLiteral*) TP)->value;
				llvm::SmallVector<llvm::Type*,2> t_args(2);
				t_args[0] = C_STRINGTYPE;
				t_args[1] = C_STRINGTYPE;
				auto CU = r.getExtern("sprintf", llvm::FunctionType::get(c_intClass.type, t_args,true));
				llvm::SmallVector<llvm::Value*,2> m_args(args.size());
				assert(args[0]);
				m_args[0] = args[0]->evalV(r, id);
				assert(m_args[0]);
				m_args[1] = r.getConstantCString(value);
				assert(m_args[1]);
				for(unsigned i=2; i<args.size(); i++){
					assert(args[i]);
					m_args[i] = args[i]->evalV(r, id);
					assert(m_args[i]);
				}
				llvm::Value* V = r.builder.CreateCall(CU, m_args);
				assert(V);
				V = r.builder.CreateSExtOrTrunc(V, intClass.type);
				assert(V);
				r.builder.GetInsertBlock()->getParent()->dump();
				cerr << endl << flush;
				return new ConstantData(V, &intClass);
			}), PositionID(0,0,"#int"));
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}


	const Data* StringLiteralClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const{
		assert(instance->type==R_STR);
		if(s=="length")
			return new IntLiteral(((StringLiteral*)instance)->value.size());
		illegalLocal(id,s);
		exit(1);
	}

#endif /* STRINGLITERALCLASSP_HPP_ */
