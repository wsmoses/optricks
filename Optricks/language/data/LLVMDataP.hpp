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


	LLVMData::LLVMData(DataType tp, const AbstractClass* const r):Data(tp),type(r){
		assert(r->classType!=CLASS_REF);
	};
	const AbstractClass* LLVMData::getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const{
			if(type->classType==CLASS_FUNC){
				return ((FunctionClass*)type)->returnType;
			}  else if(type->classType==CLASS_LAZY){
				return ((LazyClass*)type)->innerType;
			} else if(type->classType==CLASS_CLASS){
				return type;
			}	else {
				id.error("Class '"+type->getName()+"' cannot be used as function");
				exit(1);
			}
		}
const Data* LLVMData::callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const{
	if(type->classType==CLASS_FUNC){
		llvm::Value* F = getValue(r,id);
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
		FunctionProto fp("",v,fc->returnType);
		llvm::Value* V = r.builder.CreateCall(F,SingleFunction::validatePrototypeNow(&fp,r,id,args,instance));
		if(fp.returnType->classType==CLASS_VOID) return &VOID_DATA;
		else return new ConstantData(V,fp.returnType);
	} else if(type->classType==CLASS_LAZY){
		llvm::Value* F = getValue(r,id);
		assert(instance==nullptr);
		llvm::Value* V = r.builder.CreateCall(F);
		auto RT = ((LazyClass*)type)->innerType;
		if(RT->classType==CLASS_VOID) return &VOID_DATA;
		else return new ConstantData(V,RT);
	}
	else if(type->classType==CLASS_CLASS){
		llvm::Value* v = getValue(r,id);
		if(auto c = llvm::dyn_cast<llvm::ConstantInt>(v)){
			auto t = static_cast<size_t>(c->getLimitedValue());
			auto a= (const AbstractClass*)t;
			return a->callFunction(r,id,args,instance);
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
			llvm::Value* v = getValue(r,id);
			if(auto c = llvm::dyn_cast<llvm::ConstantInt>(v)){
				auto t = static_cast<size_t>(c->getLimitedValue());
				return (const AbstractClass*)t;
			} else{
				id.error("Cannot use non-constant class type");
				exit(1);
			}
		}

#endif /* LLVMDATAP_HPP_ */
