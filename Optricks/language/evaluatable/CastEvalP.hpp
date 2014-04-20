/*
 * CastEvalP.hpp
 *
 *  Created on: Feb 13, 2014
 *      Author: Billy
 */

#ifndef CASTEVALP_HPP_
#define CASTEVALP_HPP_
#include "CastEval.hpp"
#include "../class/AbstractClass.hpp"
#include "../class/builtin/FunctionClass.hpp"
#include "../class/builtin/LazyClass.hpp"
CastEval::CastEval(const Evaluatable* e, const AbstractClass* const a, PositionID d):ev(e),ac(a),id(d){
	assert((ev->getReturnType()->classType)==CLASS_LAZY == (ac->classType==CLASS_LAZY) || (d.compilerError("Lazy cast error from "+(ev->getReturnType()->getName())+" to "+ac->getName())));
	};
const AbstractClass* CastEval::getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args) const{
		if(ac->classType==CLASS_FUNC){
			return ((FunctionClass*)ac)->returnType;
		} else if(ac->classType==CLASS_LAZY){
			return ((LazyClass*)ac)->innerType;
		} else if(ac->classType==CLASS_CLASS){
			return ac;
		}	else {
			id.error("Class '"+ac->getName()+"' cannot be used as function");
			exit(1);
		}
	}

bool CastEval::hasCastValue(const AbstractClass* const a) const {
		return ac->hasCast(a);
	}
	int CastEval::compareValue(const AbstractClass* a, const AbstractClass* const b) const {
		return ac->compareValue(a,b);
	}

#endif /* CASTEVALP_HPP_ */
