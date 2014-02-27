/*
 * BoolLiteral.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef BOOLLITERAL_HPP_
#define BOOLLITERAL_HPP_
#include "Literal.hpp"
#include "../VoidData.hpp"
class BoolLiteral:public Literal{
public:
	bool const value;
	BoolLiteral(bool const val):Literal(R_BOOL),value(val){}
	virtual ~BoolLiteral(){};
	bool hasCastValue(const AbstractClass* const a) const override final{
		return a->classType==CLASS_BOOL;
	}
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
		id.error("Boolean literal cannot act as function");
		exit(1);
	}

	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(a->classType==CLASS_BOOL);
		assert(b->classType==CLASS_BOOL);
		return 0;
	}

	const Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const override{
		id.error("Boolean literal cannot be called as a function");
		return VOID_DATA;
	}
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Boolean literal cannot be used as a class");
		exit(1);
	}
	const AbstractClass* getReturnType() const override final;
	ConstantInt* getValue(RData& r, PositionID id) const override final;
	const Literal* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	ConstantInt* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final;
	void write(ostream& s,String start="") const override final{
		if(value) s << "true";
		else s << "false";
	}
};
#endif /* BOOLLITERAL_HPP_ */
