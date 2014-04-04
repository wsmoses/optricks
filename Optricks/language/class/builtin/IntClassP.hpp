/*
 * IntClass.cpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */
#ifndef INTCLASSP_HPP_
#define INTCLASSP_HPP_
#include "IntClass.hpp"
#include "FloatClass.hpp"
#include "CharClass.hpp"
#include "ComplexClass.hpp"
#include "../../RData.hpp"

/*
Value* getCharFromDigit(RData& r, PositionID id, Value* V){
	assert(dyn_cast<IntegerType>(V->getType()));
	Value*
}*/

	IntClass::IntClass(String nam, unsigned len):
		RealClass(nam, PRIMITIVE_LAYOUT,CLASS_INT,IntegerType::get(getGlobalContext(),len)){
		LANG_M->addClass(PositionID(0,0,"#int"),this);
		LANG_M->addFunction(PositionID(0,0,"#float"),"isNan")->add(
						new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
								[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
						assert(args.size()==1);
						return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));
		LANG_M->addFunction(PositionID(0,0,"#int"),"chr")->add(
				new BuiltinInlineFunction(new FunctionProto("chr",{AbstractDeclaration(this)},&charClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				return new ConstantData(r.builder.CreateSExtOrTrunc(args[0]->evalV(r, id), CHARTYPE),&charClass);}), PositionID(0,0,"#float")
			);
		/*
		LANG_M->addFunction(PositionID(0,0,"#int"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			Value* V = args[0]->evalV(r,id);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			if(auto C = dyn_cast<ConstantInt>(V)){
				String S = C->getValue().toString(10,true);
				auto F = S.length();
				for(unsigned i = 0; i<F; i++){
					r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type,S[i],false));
				}
				return VOID_DATA;
			}

			char temp[mpz_sizeinbase (value, 10) + 2];
			mpz_get_str(temp, 10, value);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			for(const char* T = temp; *T !='\0'; ++T){
				r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, *T,false));
			}
			return VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M->addFunction(PositionID(0,0,"#int"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
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
		*/
	}
inline Value* IntClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
	if(toCast->layout==LITERAL_LAYOUT) id.error("Cannot cast integer type to "+toCast->getName());
	switch(toCast->classType){
	case CLASS_INT:{
		IntClass* nex = (IntClass*)toCast;
		auto n_width = nex->getWidth();
		auto s_width = getWidth();
		if(n_width>s_width){
			return r.builder.CreateSExt(valueToCast,nex->type);
		}
		else if(n_width<s_width) id.error("Cannot cast integer type of width "+str(getWidth())+" to integer type of width"+str(nex->getWidth()));
		assert(n_width==s_width);
		return valueToCast;
		//}
	}
	case CLASS_FLOAT:{
		assert(((FloatClass*)toCast)->type);
		return r.builder.CreateSIToFP(valueToCast, toCast->type);
	}
	//case CLASS_RATIONAL:
	case CLASS_COMPLEX:{
		const RealClass* ac = ((const ComplexClass*)toCast)->innerClass;
		Value* tmp = castTo(ac, r, id, valueToCast);
		auto v = ConstantVector::getSplat(2, ac->getZero(id));
		return r.builder.CreateInsertElement(v,tmp,getInt32(0));
	}
	default:
		id.error("Cannot cast integer type ("+getName()+") to "+toCast->getName());
		exit(1);
	}
}

//todo allow rationals
int IntClass::compare(const AbstractClass* const a, const AbstractClass* const b) const{
	assert(hasCast(a));
	assert(hasCast(b));


	if(a==this) return (b==this)?0:-1;
	else if(b==this) return 1;
	else if(a->classType==CLASS_INT){
		if(b->classType!=CLASS_INT) return -1;
		IntClass* ia = (IntClass*)(a);
		IntClass* ib = (IntClass*)(b);
		if(ia->getWidth()==ib->getWidth()) return 0;
		else return (ia->getWidth() < ib->getWidth())?(-1):(1);
	}
	else if(b->classType==CLASS_INT) return 1;
	else if(a->classType==CLASS_FLOAT) return (b->classType==CLASS_FLOAT)?0:-1;
	else if(b->classType==CLASS_FLOAT) return 1;
	else if(a->classType==CLASS_COMPLEX) return (b->classType==CLASS_FLOAT)?
			compare(((ComplexClass*)a)->innerClass,((ComplexClass*)b)->innerClass):-1;
	else {
		assert(b->classType==CLASS_COMPLEX);
		return 1;
	}

}
#endif
