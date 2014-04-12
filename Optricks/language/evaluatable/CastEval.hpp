/*
 * CastEval.hpp
 *
 *  Created on: Jan 27, 2014
 *      Author: Billy
 */

#ifndef CASTEVAL_HPP_
#define CASTEVAL_HPP_
#include "Evaluatable.hpp"
class CastEval: public Evaluatable{
public:
	const Evaluatable* const ev;
	const AbstractClass* const ac;
	PositionID id;
	CastEval(const Evaluatable* e, const AbstractClass* const a, PositionID d);

	const Data* evaluate(RData& r) const override final{
		return new ConstantData(ev->evaluate(r)->castToV(r,ac,id), ac);
	}
	llvm::Value* evalV(RData& r,PositionID id) const override final{
		return ev->evaluate(r)->castToV(r,ac,id);
	}

	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* a, const AbstractClass* const b) const override final;
	const AbstractClass* getReturnType() const override final{
		return ac;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const;
};


#endif /* CASTEVAL_HPP_ */
