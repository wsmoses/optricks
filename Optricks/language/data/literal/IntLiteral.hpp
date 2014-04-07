#ifndef IntLiteral_HPP_
#define IntLiteral_HPP_
#include "Literal.hpp"
#include "FloatLiteral.hpp"
#include "../../class/literal/IntLiteralClass.hpp"
class IntLiteral:public Literal{
private:
public:
	mutable mpz_t value;
	~IntLiteral(){
		mpz_clear(value);
	}
	IntLiteral(int a, int b, int c):Literal(R_INT){
			mpz_init(value);
		}
	IntLiteral(signed long int v):Literal(R_INT){
		mpz_init_set_si(value, v);
	}

	IntLiteral(const mpz_t& val):Literal(R_INT){
		mpz_init_set(value, val);
	}
	IntLiteral(const char* str, unsigned base=10):Literal(R_INT){
		mpz_init_set_str(value, str,base);
	}
	const AbstractClass* getReturnType() const override final{
		return & intLiteralClass;
	}
	Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of integer literal");
		exit(1);
	}
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		switch(right->classType){
		case CLASS_VOID: return &VOID_DATA;
		case CLASS_INTLITERAL: return this;
		case CLASS_FLOATLITERAL:{
			return new FloatLiteral(value);
		}
		case CLASS_INT:{
			const IntClass* ic = (const IntClass*)right;
			return new ConstantData(ic->getValue(id,value), right);
		}
		case CLASS_FLOAT:{
			const FloatClass* fc = (const FloatClass*)right;
			return new ConstantData(fc->getValue(id, value), right);
		}
		case CLASS_COMPLEX:{
			const ComplexClass* cc = (const ComplexClass*)right;
			return new ConstantData(cc->getValue(id, value), right);
		}
		default:
			id.error("Integer literal cannot be cast to "+right->getName());
			exit(1);
		}
	}
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		switch(right->classType){
		case CLASS_INT:{
			IntClass* ic = (IntClass*)right;
			return ic->getValue(id,value);
		}
		case CLASS_FLOAT:{
			FloatClass* fc = (FloatClass*)right;
			return fc->getValue(id,value);
		}
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			return cc->getValue(id,value);
		}

		case CLASS_INTLITERAL:{
			id.error("Cannot get value of integer literal");
			exit(1);
		}
		case CLASS_FLOATLITERAL:{
			id.error("Cannot get value of floating-point literal");
			exit(1);
		}
		case CLASS_RATIONAL:
		default:
			id.error("Integer literal cannot be cast to "+right->getName());
			exit(1);
		}

	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return intLiteralClass.hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		return intLiteralClass.compare(a,b);
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("Integer literal cannot act as function");
		exit(1);
	}
	virtual Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override final{
		id.error("Cannot call function on int-literal");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use integer literal as class");
		exit(1);
	}
	static inline String str(const mpz_t& value){
		char temp[mpz_sizeinbase (value, 10) + 2];
		mpz_get_str(temp, 10, value);
		String s(temp);
		return s;
	}
	inline void toStream(ostream& s) const {
		char temp[mpz_sizeinbase (value, 10) + 2];
		mpz_get_str(temp, 10, value);
		s << temp;
	}
	inline String toString() const{
		return str(value);
	}
};
IntLiteral ZERO_LITERAL((signed long int)0);
IntLiteral ONE_LITERAL((signed long int)1);
IntLiteral MINUS_ONE_LITERAL((signed long int) (-1) );
IntLiteral LARGE_LITERAL("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",16);

#endif
