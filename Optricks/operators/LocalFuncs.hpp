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
	switch(cc->classType){
	case CLASS_USER:{
		auto uc = (const UserClass*)cc;
		auto lf = uc->getLocalFunction(id, s, v);
		if(lf==nullptr) return &voidClass;
		return lf->getSingleProto()->returnType;
	}
	default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}

const Data* getLocalFunction(RData& r, PositionID id, String s, const Data* inst, const std::vector<const Evaluatable*>& v){
	assert(inst);
	const AbstractClass* cc = inst->getReturnType();
	switch(cc->classType){
		case CLASS_USER:{
			auto uc = (const UserClass*)cc;
			auto lf = uc->getLocalFunction(id, s, v);
			if(lf==nullptr) return &VOID_DATA;
			return lf->callFunction(r,id, v, inst);
		}
		default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}



#endif /* LOCALFUNCS_HPP_ */
