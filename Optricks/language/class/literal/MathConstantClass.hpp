/*
 * MathConstantLiteral.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef MATHCONSTANTCLASS_HPP_
#define MATHCONSTANTCLASS_HPP_

#include "../AbstractClass.hpp"

class MathConstantClass: public AbstractClass{
protected:
	inline String fromC(MathConstant mc){
		switch(mc){
		case MATH_PI:
			return "PiClass";
		case MATH_E:
			return "EClass";
		case MATH_EULER_MASC:
			return "EulerGammaClass";
		case MATH_LN2:
			return "Log2Class";
		case MATH_CATALAN:
			return "CatalanClass";
		}
	}
public:
	MathConstant mathType;
	MathConstantClass(MathConstant mc):
		AbstractClass(nullptr,fromC(mc),nullptr,PRIMITIVE_LAYOUT,CLASS_MATHLITERAL,true,BOOLTYPE)
		,mathType(mc){
		LANG_M.addFunction(PositionID(0,0,"#float"),"isNan")->add(new BuiltinInlineFunction(
				new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
				[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
		assert(args.size()==1);
		return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));

		///register methods such as print / tostring / tofile / etc
	}
	inline bool hasCast(const AbstractClass* const toCast) const{
		if(toCast==this) return true;
		switch(toCast->classType){
		case CLASS_VOID: return true;
		case CLASS_FLOATLITERAL: return true;
		case CLASS_COMPLEX:{
			return hasCast(((const ComplexClass*)toCast)->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		default:
			return false;
		}
	}

	AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		illegalLocal(id,s);
		exit(1);
	}
	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		illegalLocal(id,s);
		exit(1);
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		if(a==this) return (b==this)?0:-1;
		else if(b==this) return 1;
		if(a->classType==CLASS_FLOAT) return (b->classType==CLASS_FLOAT)?0:-1;
		else if(b->classType==CLASS_FLOAT) return 1;
		assert(a->classType==CLASS_COMPLEX);
		assert(b->classType==CLASS_COMPLEX);
		return compare(((const ComplexClass*)a)->innerClass,((const ComplexClass*)b)->innerClass);
	}
};

#endif /* MATHCONSTANTLITERAL_HPP_ */
