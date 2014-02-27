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

		bool Statement::hasCastValue(const AbstractClass* const a) const {
			return getReturnType()->hasCast(a);
		}
		int Statement::compareValue(const AbstractClass* const a, const AbstractClass* const b) const{
			return getReturnType()->compareValue(a,b);
		}


#endif /* STATEMENTP_HPP_ */
