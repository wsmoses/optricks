#ifndef IntLiteral_HPP_
#define IntLiteral_HPP_
#include "Literal.hpp"
#include "../../class/literal/IntLiteralClass.hpp"
class IntLiteral:public Literal{
private:
public:
	/**
	 * If type is null, then this can be any long/integer type
	 */
	IntLiteralClass* const intType;
	//IntLiteral(const mpz_t& val, IntClass* cp=NULL):Literal(R_INT),value(val),intType(cp){ assert(val);}
	IntLiteral(signed long int v):Literal(R_INT),
			intType(IntLiteralClass::get(v)){
	}
	IntLiteral(const mpz_t& val):Literal(R_INT),
			intType(IntLiteralClass::get(val)){
	}
	IntLiteral(const char* str, unsigned base, IntClass* cp=NULL):Literal(R_INT),
			intType(IntLiteralClass::get(str,base)){
	}
	const AbstractClass* getReturnType() const override final{
		return intType;
	}
	Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of integer literal");
		exit(1);
	}
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right==this) return this;
		switch(right->classType){
		case CLASS_INT:{
			IntClass* ic = (IntClass*)right;
			return new ConstantData(ic->getValue(id,intType->value), right);
		}
		case CLASS_FLOAT:{
			FloatClass* fc = (FloatClass*)right;
			return new ConstantData(fc->getValue(id, intType->value), right);
		}
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			const Data* real = castTo(r, cc->innerClass, id);
			return new ComplexLiteral(real, right);
		}
		case CLASS_RATIONAL:
		default:
			id.error("Integer literal cannot be cast to "+right->name);
			exit(1);
		}
	}
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		switch(right->classType){
		case CLASS_INT:{
			IntClass* ic = (IntClass*)right;
			return ic->getValue(id,intType->value);
		}
		case CLASS_FLOAT:{
			FloatClass* fc = (FloatClass*)right;
			return fc->getValue(id, intType->value);
		}
		case CLASS_COMPLEX:{
			ComplexClass* cc = (ComplexClass*)right;
			const Data* real = castTo(r, cc->innerClass, id);
			return cc->getValue(id, intType->value);
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
			id.error("Integer literal cannot be cast to "+right->name);
			exit(1);
		}

	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return intType->hasCast(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		return intType->compare(a,b);
	}
	static inline IntLiteral* getZero() {
		static IntLiteral* zero = new IntLiteral((signed long int)0,nullptr);
		return zero;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		id.error("Integer literal cannot act as function");
		exit(1);
	}
	void write(ostream& s, String st="") const override final{
		auto tmp =  mpz_get_str(nullptr, 10, intType->value);
		void (*freefunc)(void*,size_t);
		s << tmp;
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
	}
	virtual Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override final{
		id.error("Cannot call function on int-literal");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use integer literal as class");
		exit(1);
	}
};

/*
 * class IntLiteral:public Literal{
private:
public:
	mutable mpz_t value;
	IntClass* const intType;
	//IntLiteral(const mpz_t& val, IntClass* cp=NULL):Literal(R_INT),value(val),intType(cp){ assert(val);}
	IntLiteral(signed long int v, IntClass* cp=nullptr):Literal(R_INT),value(),intType(cp){
		mpz_init_set_si(value,v);
	}
	IntLiteral(mpz_t val, IntClass* cp):Literal(R_INT),value(),intType(cp){
		mpz_init_set(value,val);
	}
	IntLiteral(IntClass* cp):Literal(R_INT),value(),intType(cp){
		mpz_init(value);
	}
	IntLiteral(const char* str, unsigned base, IntClass* cp=NULL):Literal(R_INT),value(),intType(cp){
		mpz_init_set_str(value,str,base);
	}
	const AbstractClass* getReturnType() const override final;
	ConstantInt* getValue(RData& r, PositionID id) const override final;
	const Literal* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	virtual ~IntLiteral(){
		mpz_clear(value);
	}
	static inline IntLiteral* getZero() {
		static IntLiteral* zero = new IntLiteral((signed long int)0,nullptr);
		return zero;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		id.error("Integer literal cannot act as function");
		exit(1);
	}
	void write(ostream& s, String st="") const override final{
		auto tmp =  mpz_get_str(nullptr, 10, value);
		void (*freefunc)(void*,size_t);
		s << tmp;
		mp_get_memory_functions(nullptr, nullptr, &freefunc);
		freefunc(tmp, strlen(tmp)+1);
	}
	virtual Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override final{
		id.error("Cannot call function on int-literal");
		exit(1);
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use integer literal as class");
		exit(1);
	}
};
 */
/*const AbstractClass* maxClass(IntLiteral* l,IntLiteral* r){
	if(! l->intType) return r;
	if(! r->intType) return l;
	if(l->intType->isSigned != r->intType->isSigned){
		if(l->intType->isSigned){
			if(l->intType->getWidth()>= r->intType->getWidth()+1) return l;
			else return A;
		} else {
			if(r->intType->getWidth()>= l->intType->getWidth()+1) return r;
			else return A;
		}
	} else{
		if(l->intType->getWidth() >= r->intType->getWidth())
			return l;
		else
			return r;
	}
}*/
#endif
