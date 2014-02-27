/*
 * ComplexLiteral.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef COMPLEXLITERAL_HPP_
#define COMPLEXLITERAL_HPP_
#include "Literal.hpp"
#include "../VoidData.hpp"
//TODO implement methods
class ComplexLiteral:public Literal{
public:
	const Literal* const real;
	const Literal* const imag;
	/**
	 * If type is null, then this can be any long/integer type
	 */
	const ComplexClass* const complexType;
	virtual ~ComplexLiteral(){};
	const Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override{
			id.error("Cannot use complex literal as function");
			return VOID_DATA;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		id.error("Complex literal cannot act as function");
		exit(1);
	}
		const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
			id.error("Cannot use complex literal as class");
			exit(1);
		}
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	ComplexLiteral(const Literal* const r, const Literal* const i, const ComplexClass* const cp=NULL):Literal(R_COMPLEX),real(r),imag(i),complexType(cp){
		assert(r); assert(i);
		assert(r->type==R_INT || r->type==R_FLOAT);
		assert(i->type==R_INT || i->type==R_FLOAT);
	}
	const AbstractClass* getReturnType() const override final;
	Constant* getValue(RData& r, PositionID id) const override final;
	const ComplexLiteral* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	Constant* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	void write(ostream& s,String start="") const override final{
		s << "complex(";
		real->write(s,start);
		s << ", ";
		imag->write(s,start);
		s << ")";
	}
};




#endif /* COMPLEXLITERAL_HPP_ */
