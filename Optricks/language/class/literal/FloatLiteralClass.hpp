/*
 * FloatLiteralClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef FLOATLITERALCLASS_HPP_
#define FLOATLITERALCLASS_HPP_
#include "../AbstractClass.hpp"
#include "../builtin/RealClass.hpp"

class FloatLiteralClass: public RealClass{
public:
	FloatLiteralClass(bool b):
		RealClass("floatLiteral",LITERAL_LAYOUT,CLASS_FLOATLITERAL,llvm::IntegerType::get(getGlobalContext(), 1)){

		/*LANG_M->addFunction(PositionID(0,0,"#float"),"isNan")->add(
				new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
				assert(args.size()==1);
				return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));
	*/
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_FLOATLITERAL: return toCast==this;
		case CLASS_RATIONAL:
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
		assert(a->classType==CLASS_FLOAT || a==this );
		assert(b->classType==CLASS_FLOAT || b==this);
		if(a==this){
			if(b==this) return 0;
			else return -1;
		} else if(b==this) return 1;
		else return 0;
	}
	Constant* getZero(PositionID id, bool negative=false) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	Constant* getOne(PositionID id) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
	Constant* getValue(PositionID id, const mpz_t& c) const{
		id.compilerError("Cannot convert float-literal to llvm type");
		exit(1);
	}
};

const FloatLiteralClass floatLiteralClass(true);
#endif /* FLOATLITERALCLASS_HPP_ */
