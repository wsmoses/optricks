/*
 * OBJ_OBJ.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef OBJ_OBJ_HPP_
#define OBJ_OBJ_HPP_
#include "../O_Expression.hpp"

class OBJ_OBJ : public Expression{
	public:
		virtual String toString() = 0;

		ostream& write(ostream& f){
			return f << "OBJECT(" << toString() << ", key=" << getToken() << ')';
		}
};

#include "OBJ_NULL.hpp"
#include "OBJ_ARR.hpp"
#include "OBJ_DEC.hpp"
#include "OBJ_INT.hpp"
#include "OBJ_SLICE.hpp"
#include "OBJ_STR.hpp"
#endif /* OBJ_OBJ_HPP_ */
