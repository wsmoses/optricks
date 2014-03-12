/*
 * LLVMDataP.hpp
 *
 *  Created on: Jan 20, 2014
 *      Author: Billy
 */

#ifndef LLVMDATAP_HPP_
#define LLVMDATAP_HPP_
#include "./LLVMData.hpp"
#include "../class/AbstractClass.hpp"
#include "../class/builtin/FunctionClass.hpp"


	const AbstractClass* LLVMData::getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const{
			if(type->classType==CLASS_FUNC){
				return ((FunctionClass*)type)->returnType;
			}  else if(type->classType==CLASS_CLASS){
				return type;
			}	else {
				id.error("Class '"+type->getName()+"' cannot be used as function");
				exit(1);
			}
		}
const Data* LLVMData::callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const{
	if(type->classType==CLASS_FUNC){
		Value* F = getValue(r,id);
		FunctionClass* fc = (FunctionClass*)type;

		std::vector<AbstractDeclaration> v;
		if(fc->isVarArg){
			id.compilerError("cannot make var arg to proto");
			assert(0);
			exit(1);
		}
		for(const AbstractClass* a: fc->argumentTypes){
			v.push_back(AbstractDeclaration(a));
		}
		FunctionProto fp("#data",v,fc->returnType);
		assert(F);
		assert(dyn_cast<Function>(F));
		return new ConstantData(r.builder.CreateCall(F,SingleFunction::validatePrototypeNow(&fp,r,id,args)),fp.returnType);
	}
	else if(type->classType==CLASS_CLASS){
		Value* v = getValue(r,id);
		if(ConstantInt* c = dyn_cast<ConstantInt>(v)){
			auto t = c->getLimitedValue();
			AbstractClass* a= (AbstractClass*)(  ((size_t)t)  );
			return a->callFunction(r,id,args);
		} else{
			id.error("Cannot use non-constant class type");
			exit(1);
		}
	} else{
		id.error("Cannot use non-function type as function");
		exit(1);
	}
}
	bool LLVMData::hasCastValue(const AbstractClass* const a) const {
			return type->hasCast(a);
		}
		int LLVMData::compareValue(const AbstractClass* a, const AbstractClass* const b) const {
			return type->compare(a, b);
		}

		const AbstractClass* LLVMData::getMyClass(RData& r, PositionID id) const{
			if(type->classType!=CLASS_CLASS) id.error("Cannot use non-class type as a class");
			Value* v = getValue(r,id);
			if(ConstantInt* c = dyn_cast<ConstantInt>(v)){
				auto t = c->getLimitedValue();
				return (AbstractClass*)(  ((size_t)t)  );
			} else{
				id.error("Cannot use non-constant class type");
				exit(1);
			}
		}

#endif /* LLVMDATAP_HPP_ */
