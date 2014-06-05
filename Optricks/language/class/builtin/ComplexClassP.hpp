/*
 * ComplexClassP.hpp
 *
 *  Created on: Jan 29, 2014
 *      Author: Billy
 */

#ifndef COMPLEXCLASSP_HPP_
#define COMPLEXCLASSP_HPP_
#include "./ComplexClass.hpp"
#include "../literal/FloatLiteralClass.hpp"
#include "../../data/literal/ImaginaryLiteral.hpp"
#include "../../data/literal/FloatLiteral.hpp"
#include "../../data/literal/IntLiteral.hpp"
#include "../../data/ConstantData.hpp"
#include "../../data/LocationData.hpp"

ComplexClass::ComplexClass(String name, const RealClass* inner, bool reg):
		AbstractClass(nullptr,name, nullptr,PRIMITIVE_LAYOUT,CLASS_COMPLEX,true,llvm::VectorType::get(cType(inner),2)),innerClass(inner){
		assert(inner);
		assert(inner->classType!=CLASS_COMPLEX);
		assert(inner->classType==CLASS_INT || inner->classType==CLASS_FLOAT || inner->classType==CLASS_INTLITERAL || inner->classType==CLASS_FLOATLITERAL);
		if(reg) LANG_M.addClass(PositionID(0,0,"#complex"),this);
		LANG_M.addFunction(PositionID(0,0,"#complex"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&floatLiteralClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			const Data* D = args[0]->evaluate(r);
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->innerClass}).first->callFunction(r, id, {this->getLocalData(r, id, "real", D)}, nullptr);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			r.builder.CreateCall(CU, getInt32('+'));
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->innerClass}).first->callFunction(r, id, {this->getLocalData(r, id, "imag", D)}, nullptr);
			r.builder.CreateCall(CU, getInt32('j'));
			return &VOID_DATA;
		}), PositionID(0,0,"#complex"));
		LANG_M.addFunction(PositionID(0,0,"#complex"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&floatLiteralClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			const Data* D = args[0]->evaluate(r);
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->innerClass}).first->callFunction(r, id, {this->getLocalData(r, id, "real", D)}, nullptr);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			r.builder.CreateCall(CU, getInt32('+'));
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->innerClass}).first->callFunction(r, id, {this->getLocalData(r, id, "imag", D)}, nullptr);
			r.builder.CreateCall(CU, getInt32('j'));
			r.builder.CreateCall(CU, getInt32('\n'));
			return &VOID_DATA;
		}), PositionID(0,0,"#complex"));
		if(inner->classType==CLASS_FLOATLITERAL){
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs")->add(
				new BuiltinInlineFunction(new FunctionProto("abs",{AbstractDeclaration(this)},&floatLiteralClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				FloatLiteral* out = new FloatLiteral(0,0,0);
				const ImaginaryLiteral* il = (const ImaginaryLiteral*) args[0]->evaluate(r);
				mpfr_mul(out->value, ((FloatLiteral*) il->imag)->value,((FloatLiteral*) il->imag)->value, MPFR_RNDN);
				if(il->real){
					mpfr_t tmp;
					mpfr_init(tmp);
					mpfr_mul(tmp, ((FloatLiteral*) il->real)->value,((FloatLiteral*) il->real)->value, MPFR_RNDN);
					mpfr_add(out->value, out->value, tmp, MPFR_RNDN);
				}
				mpfr_sqrt(out->value, out->value, MPFR_RNDN);
				return out;
			}), PositionID(0,0,"#complex"));
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs2")->add(
				new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},&floatLiteralClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				FloatLiteral* out = new FloatLiteral(0,0,0);
				const ImaginaryLiteral* il = (const ImaginaryLiteral*) args[0]->evaluate(r);
				mpfr_mul(out->value, ((FloatLiteral*) il->imag)->value,((FloatLiteral*) il->imag)->value, MPFR_RNDN);
				if(il->real){
					mpfr_t tmp;
					mpfr_init(tmp);
					mpfr_mul(tmp, ((FloatLiteral*) il->real)->value,((FloatLiteral*) il->real)->value, MPFR_RNDN);
					mpfr_add(out->value, out->value, tmp, MPFR_RNDN);
				}
				return out;
			}), PositionID(0,0,"#complex"));
		} else if(inner->classType==CLASS_INTLITERAL){
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs2")->add(
				new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},&intLiteralClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				IntLiteral* out = new IntLiteral(0,0,0);
				const ImaginaryLiteral* il = (const ImaginaryLiteral*) args[0]->evaluate(r);
				mpz_mul(out->value, ((IntLiteral*) il->imag)->value,((IntLiteral*) il->imag)->value);
				if(il->real){
					mpz_t tmp;
					mpz_init(tmp);
					mpz_mul(tmp, ((IntLiteral*) il->real)->value,((IntLiteral*) il->real)->value);
					mpz_add(out->value, out->value, tmp);
				}
				return out;
			}), PositionID(0,0,"#complex"));
		}
		else if(inner->classType==CLASS_FLOAT){
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs")->add(
				new BuiltinInlineFunction(new FunctionProto("abs",{AbstractDeclaration(this)},innerClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				llvm::Value* V = args[0]->evalV(r, id);
				V = r.builder.CreateFMul(V, V);
				V = r.builder.CreateFAdd(r.builder.CreateExtractElement(V, getInt32(0)),r.builder.CreateExtractElement(V, getInt32(1)));
				V = r.builder.CreateCall(llvm::Intrinsic::getDeclaration(r.lmod, llvm::Intrinsic::sqrt, llvm::SmallVector<llvm::Type*,1>(1,innerClass->type)),V);
				return new ConstantData(V, innerClass);
			}), PositionID(0,0,"#complex"));
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs2")->add(
				new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},innerClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				llvm::Value* V = args[0]->evalV(r, id);
				V = r.builder.CreateFMul(V, V);
				V = r.builder.CreateFAdd(r.builder.CreateExtractElement(V, getInt32(0)),r.builder.CreateExtractElement(V, getInt32(1)));
				return new ConstantData(V, innerClass);
			}), PositionID(0,0,"#complex"));
		} else if(inner->classType==CLASS_INT){
			LANG_M.addFunction(PositionID(0,0,"#complex"),"abs2")->add(
				new BuiltinInlineFunction(new FunctionProto("abs2",{AbstractDeclaration(this)},innerClass),
				[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				llvm::Value* V = args[0]->evalV(r, id);
				V = r.builder.CreateMul(V, V);
				V = r.builder.CreateAdd(r.builder.CreateExtractElement(V, getInt32(0)),r.builder.CreateExtractElement(V, getInt32(1)));
				return new ConstantData(V, innerClass);
			}), PositionID(0,0,"#complex"));
		}
	}

const Data* ComplexClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const{
	if(s!="real" && s!="imag"){
		illegalLocal(id,s);
		exit(1);
	}
	assert(instance->type==R_IMAG || instance->type==R_LOC || instance->type==R_CONST);
	assert(instance->getReturnType()==this);
	if(instance->type==R_IMAG){
		ImaginaryLiteral* cl = (ImaginaryLiteral*)instance;
		if(s=="real") return new ConstantData(innerClass->getZero(id), innerClass);
		else{
			return cl->imag->castTo(r, innerClass, id);
		}
	} else if(instance->type==R_CONST){
		llvm::Value* v = ((ConstantData*)instance)->value;
		return new ConstantData(r.builder.CreateExtractElement(v,getInt32((s=="real")?0:1)),innerClass);

	} else {
		assert(instance->type==R_LOC);
		auto LD = ((const LocationData*)instance)->value;
		return new LocationData(LD->getInner(r, id, 0, (s=="real")?0:1), innerClass);
	}
	exit(1);
}


#endif /* COMPLEXCLASSP_HPP_ */
