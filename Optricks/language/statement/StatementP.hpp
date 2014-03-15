/*
 * StatementP.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef STATEMENTP_HPP_
#define STATEMENTP_HPP_
#include "Statement.hpp"
#include "../class/AbstractClass.hpp"
#include "../data/Data.hpp"
		Value* Statement::evalCastV(RData& r,AbstractClass* c, PositionID id){
			return evaluate(r)->castToV(r,c,id);
		}
		bool Statement::hasCastValue(const AbstractClass* const a) const {
			return getReturnType()->hasCast(a);
		}
		int Statement::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
			return getReturnType()->compare(a,b);
		}


#endif /* STATEMENTP_HPP_ */
