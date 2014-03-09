/*
 * Evaluatable.hpp
 *
 *  Created on: Jan 26, 2014
 *      Author: Billy
 */

#ifndef EVALUATABLE_HPP_
#define EVALUATABLE_HPP_
#include "../includes.hpp"
#define EVALUATABLE_C_
class Evaluatable{
public:
	/**
	* Creates LLVM IR for all pieces (either by lazy evaluation or immediately).
	*/
	virtual ~Evaluatable(){};
	virtual const Data* evaluate(RData& r) const = 0;
	virtual llvm::Value* evalV(RData& r,PositionID id) const;
	virtual const AbstractClass* getReturnType() const =0;
	virtual const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const=0;
	virtual bool hasCastValue(const AbstractClass* const a) const=0;
	virtual int compareValue(const AbstractClass* const a, const AbstractClass* const b) const=0;
};



#endif /* EVALUATABLE_HPP_ */
