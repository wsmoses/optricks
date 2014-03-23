/*
 * IntLiteralClass.hpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Billy
 */

#ifndef INTLITERALCLASS_HPP_
#define INTLITERALCLASS_HPP_
#include "../AbstractClass.hpp"


	struct mpzCompare
	{
	    bool operator() (const mpz_t& val1, const mpz_t& val2) const
	    {
	    	return mpz_cmp(val1, val2) > 0;
	    }
	};
class IntLiteralClass: public RealClass{
public:
	IntLiteralClass(bool b):
		RealClass("intLiteral",LITERAL_LAYOUT,CLASS_INTLITERAL,llvm::IntegerType::get(getGlobalContext(), 1))
		{
		/*
		LANG_M->addFunction(PositionID(0,0,"#float"),"isNan")->add(
						new BuiltinInlineFunction(new FunctionProto("isNan",{AbstractDeclaration(this)},&boolClass),
								[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
						assert(args.size()==1);
						return new ConstantData(BoolClass::getValue(false),&boolClass);}), PositionID(0,0,"#float"));
		*/
		///register methods such as print / tostring / tofile / etc
		//check to ensure that you can pass mpz_t like that instead of using _init
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_COMPLEX:{
			ComplexClass* ic = (ComplexClass*)toCast;
			return hasCast(ic->innerClass);
		}
		case CLASS_FLOATLITERAL:
		case CLASS_FLOAT:{
			return true;
		}
		case CLASS_INTLITERAL: return toCast==this;
		case CLASS_INT: return true;
		case CLASS_RATIONAL:
		default:
			return false;
		}
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		//todo allow complex/floats as well
		assert(hasCast(a));
		assert(hasCast(b));
		if(a==this) return (b==this)?0:-1;
		else if(b==this) return 1;
		if(a->classType==CLASS_INT) return (b->classType==CLASS_INT)?0:-1;
		else if(b->classType==CLASS_INT) return 1;
		if(a->classType==CLASS_FLOATLITERAL) return (b->classType==CLASS_FLOATLITERAL)?0:-1;
		else if(b->classType==CLASS_FLOATLITERAL) return 1;
		if(a->classType==CLASS_FLOAT) return (b->classType==CLASS_FLOAT)?0:-1;
		else if(b->classType==CLASS_FLOAT) return 1;
		if(a->classType==CLASS_RATIONAL) return (b->classType==CLASS_RATIONAL)?0:-1;
		else if(b->classType==CLASS_RATIONAL) return 1;
		if(a->classType==CLASS_COMPLEX) return (b->classType==CLASS_COMPLEX)?compare(((const ComplexClass*)a)->innerClass, ((const ComplexClass*)b)->innerClass):-1;
		else{
			assert(b->classType==CLASS_COMPLEX);
			return 1;
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

	Constant* getZero(PositionID id, bool negative=false) const{
		id.compilerError("Cannot convert int-literal to llvm type");
		exit(1);
	}
	Constant* getOne(PositionID id) const{
		id.compilerError("Cannot convert int-literal to llvm type");
		exit(1);
	}
	Constant* getValue(PositionID id, const mpz_t& c) const{
		id.compilerError("Cannot convert int-literal to llvm type");
		exit(1);
	}
};

const IntLiteralClass intLiteralClass(true);

#endif /* INTLITERALCLASS_HPP_ */
