#ifndef IntLiteral_HPP_
#define IntLiteral_HPP_
#include "Literal.hpp"
#include "FloatLiteral.hpp"
#include "../../class/literal/IntLiteralClass.hpp"
class IntLiteral:public Literal{
private:
public:

	/*static IntLiteralClass* get(const mpz_t& l) {
		static std::map<const mpz_t, IntLiteralClass*, mpzCompare> mmap;
		auto find = mmap.find(l);
		if(find==mmap.end()){
			mpz_t mt;
			mpz_init_set(mt,l);
			return mmap.insert(std::pair<const mpz_t, IntLiteralClass*>(mt,new IntLiteralClass(mt))).first->second;
		}
		else return find->second;
	}
	static inline IntLiteralClass* get(const char* str, unsigned base) {
		mpz_t value;
		mpz_init_set_str(value,str,base);
		auto tmp = get(value);
		mpz_clear(value);//should check
		return tmp;
	}
	static inline IntLiteralClass* get(signed long int val) {
		mpz_t value;
		mpz_init_set_si(value,val);
		auto tmp = get(value);
		mpz_clear(value);//should check
		return tmp;
	}*/
	mutable mpz_t value;
	~IntLiteral(){
		mpz_clear(value);
	}
	IntLiteral(signed long int v):Literal(R_INT){
		mpz_init_set_si(value, v);
	}

	IntLiteral(const mpz_t& val):Literal(R_INT){
		mpz_init_set(value, val);
	}
	IntLiteral(const char* str, unsigned base):Literal(R_INT){
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
		case CLASS_INTLITERAL: return this;
		case CLASS_FLOATLITERAL:
			return new FloatLiteral(value);
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
		auto tmp =  mpz_get_str(temp, 10, value);
		String s(tmp);
		return s;
	}
	inline String toString() const{
		return str(value);
	}
};
IntLiteral ZERO_LITERAL((signed long int)0);
IntLiteral ONE_LITERAL((signed long int)0);
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
