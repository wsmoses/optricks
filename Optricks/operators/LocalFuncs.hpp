/*
 * LocalFuncs.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef LOCALFUNCS_HPP_
#define LOCALFUNCS_HPP_

#include "../language/class/AbstractClass.hpp"

const AbstractClass* getLocalFunctionReturnType(PositionID id, String s, const AbstractClass* cc, const std::vector<const Evaluatable*>& v){

}

const Data* getLocalFunction(RData&, PositionID id, String s, const Data* inst, const std::vector<const Evaluatable*>& v){
	const AbstractClass* cc = inst->getReturnType();
}



#endif /* LOCALFUNCS_HPP_ */
