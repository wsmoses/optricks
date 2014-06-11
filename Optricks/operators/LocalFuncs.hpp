/*
 * LocalFuncs.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef LOCALFUNCS_HPP_
#define LOCALFUNCS_HPP_

#include "../language/class/AbstractClass.hpp"

bool hasLocalFunction(String s, const AbstractClass* cc){
	switch(cc->classType){
	case CLASS_USER:{
		auto uc = (const UserClass*)cc;
		return uc->hasLocalFunction(s);
	}
	default:
		return false;
	}
}
const AbstractClass* getLocalFunctionReturnType(PositionID id, String s, const AbstractClass* cc, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v){
	switch(cc->classType){
	case CLASS_USER:{
		auto uc = (const UserClass*)cc;
		auto lf = uc->getLocalFunction(id, s, t_args, v);
		if(lf==nullptr) return &voidClass;
		return lf->getSingleProto()->returnType;
	}
	default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}

const Data* getLocalFunction(RData& r, PositionID id, String s, const Data* inst, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v){
	assert(inst);
	const AbstractClass* cc = inst->getReturnType();
	switch(cc->classType){
		case CLASS_USER:{
			auto uc = (const UserClass*)cc;
			auto lf = uc->getLocalFunction(id, s, t_args, v);
			if(lf==nullptr) return &VOID_DATA;
			return lf->callFunction(r,id, v, inst);
		}
		default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}



#endif /* LOCALFUNCS_HPP_ */
