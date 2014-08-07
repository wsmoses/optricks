/*
 * LazyWrapperData.hpp
 *
 *  Created on: Apr 19, 2014
 *      Author: Billy
 */

#ifndef LAZYWRAPPERDATA_HPP_
#define LAZYWRAPPERDATA_HPP_

#include "./Data.hpp"
class LazyWrapperData:public Data{
public:
	const Evaluatable* const value;
	LazyWrapperData(const Evaluatable* const val):
		Data(R_LAZY),value(val){
		assert(val);
	}
	const AbstractClass* getMyClass(PositionID id) const override final{
		id.error("Cannot use reference as class");
		exit(1);
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		if(a->classType!=CLASS_LAZY) return false;
		return value->hasCastValue(a);
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		return value->compareValue(a, b);
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* instance) const override final{
		if(args.size()!=0){
			id.error("Cannot evaluate lazy as function with arguments");
			exit(1);
		}
		assert(instance==nullptr);
		return value->evaluate(r);
	}
	const AbstractClass* getReturnType() const override final{
		return LazyClass::get(value->getReturnType());
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b)const override final{
		assert(b==false);
		return value->getReturnType();
	}

	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		id.compilerError("Cannot get value of reference");
		exit(1);
	}
	inline void setValue(RData& r, llvm::Value* v) const{
		PositionID(0,0,"#lazy").compilerError("Cannot set value of lazy");
		exit(1);
	}
	inline const Data* toValue(RData& r,PositionID id) const override final{
		return this;
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType!=CLASS_LAZY)
			id.compilerError("Cannot cast lazy class to non-lazy class");
		auto RL = (const LazyClass*)right;
		if(value->getReturnType()==RL->innerType) return this;
		else return new LazyWrapperData(new CastEval(value, right, id));
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType!=CLASS_LAZY)
				id.compilerError("Cannot cast lazy class to non-lazy class");
		auto lc = (const LazyClass*)right;
		llvm::BasicBlock* Parent = r.builder.GetInsertBlock();
		llvm::FunctionType* FT = (llvm::FunctionType*)(((llvm::PointerType*)lc->type)->getElementType());
		llvm::Function* F = llvm::Function::Create(FT,LOCAL_FUNC,"%lazy",r.lmod);
		llvm::BasicBlock* BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		const Data* D = value->evaluate(r);
		if(lc->innerType->classType==CLASS_VOID)
			r.builder.CreateRetVoid();
		else
			r.builder.CreateRet(D->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
		return F;
	}
};




#endif /* LAZYWRAPPERDATA_HPP_ */
