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
	MathConstant mathType;
	MathConstantClass(MathConstant mc):
		AbstractClass(nullptr,fromC(mc),nullptr,LITERAL_LAYOUT,CLASS_MATHLITERAL,true,nullptr){
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
public:
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
		assert(a->classType==CLASS_COMPLEX || a->classType==CLASS_FLOAT || a==this );
		assert(b->classType==CLASS_COMPLEX || a->classType==CLASS_FLOAT|| b==this);
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
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
