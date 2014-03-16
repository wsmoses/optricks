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
		case MATH_NAN:
			return "NanClass";
		case MATH_P_INF:
			return "InfClass";
		case MATH_N_INF:
			return "-InfClass";
		}
	}
	MathConstantClass(MathConstant mc):
		AbstractClass(nullptr,fromC(mc),nullptr,LITERAL_LAYOUT,CLASS_MATHLITERAL,true,BOOLTYPE){
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
public:
	MathConstant mathType;
	inline bool hasCast(const AbstractClass* const toCast) const{
		if(toCast==this) return true;
		switch(toCast->classType){
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
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		id.compilerError("This method should never be called.....ever....");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(hasCast(a));
		assert(hasCast(b));
		if(a==this) return (b==this)?0:-1;
		else if(b==this) return 1;
		if(a->classType==CLASS_FLOAT) return (b->classType==CLASS_FLOAT)?0:-1;
		else if(b->classType==CLASS_FLOAT) return 1;
		return 0;
	}
	static MathConstantClass* get(MathConstant mc) {
		static std::map<MathConstant,MathConstantClass> m;
		auto find = m.find(mc);
		if(find!=m.end()) return & (find->second);
		auto tmp = m.insert(std::pair<MathConstant, MathConstantClass>(mc, MathConstantClass(mc)));
		return & tmp.first->second;
	}
};

#endif /* MATHCONSTANTLITERAL_HPP_ */
