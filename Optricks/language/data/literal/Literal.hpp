/*
 * Literal.hpp
 *
 *  Created on: Jan 4, 2014
 *      Author: Billy
 */

#ifndef LITERAL_HPP_
#define LITERAL_HPP_
#include "../Data.hpp"
#include "../../statement/Statement.hpp"
class Literal: public Data, public Statement{
public:
	inline Literal(DataType r): Data(r),Statement(){};
	//virtual Constant* getValue(RData& r, PositionID id) const override=0;
	//virtual const Literal* castTo(RData& r, const AbstractClass* const right, PositionID id) const override=0;
	virtual llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override=0;

	inline const Literal* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{

	}
	const Literal* evaluate(RData& a) const override final{
		return this;
	}
	void registerClasses() const override final{};
	void registerFunctionPrototype(RData& r) const override final{};
	void buildFunction(RData& r) const override final{};
	const Token getToken() const override{ return T_LITERAL; };

	virtual const AbstractClass* getReturnType() const =0;

	virtual bool hasCastValue(const AbstractClass* const a) const=0;
	virtual int compareValue(const AbstractClass* const a, const AbstractClass* const b) const=0;
};


#endif /* LITERAL_HPP_ */
