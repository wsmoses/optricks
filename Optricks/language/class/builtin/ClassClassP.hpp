/*
 * ClassClassP.hpp
 *
 *  Created on: May 25, 2014
 *      Author: Billy
 */

#ifndef CLASSCLASSP_HPP_
#define CLASSCLASSP_HPP_
#include "./ClassClass.hpp"

#include "../literal/IntLiteralClass.hpp"
#include "../../data/literal/IntLiteral.hpp"


	const AbstractClass* ClassClass::getLocalReturnClass(PositionID id, String s) const{
		illegalLocal(id,s);
		exit(1);
	}
const Data* ClassClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const{

	illegalLocal(id,s);
	exit(1);
}


#endif /* CLASSCLASSP_HPP_ */
