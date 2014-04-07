/*
 * ArrayData.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: Billy
 */

#ifndef ARRAYDATA_HPP_
#define ARRAYDATA_HPP_


#include "Data.hpp"
#include "../class/builtin/ArrayClass.hpp"
#include "../class/ClassLib.hpp"
#include "../class/builtin/ClassClass.hpp"
#include "./VoidData.hpp"
class ArrayData:public Data{
public:
	PositionID filePos;
	const std::vector<const Data*> inner;
	ArrayData(const std::vector<const Data*>& vec, PositionID id):Data(R_ARRAY),filePos(id),inner(vec){};
	const AbstractClass* getReturnType() const override final{
		std::vector<const AbstractClass*> vec;
		const AbstractClass* A = (inner.size()==0)?nullptr:inner[0]->getReturnType();
		for(int i=1; i<inner.size(); i++){
			A = getMin(A, inner[0]->getReturnType(),filePos);
		}
		return ArrayClass::get(A, inner.size());
	}
	inline const ConstantData* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		return new ConstantData(castToV(r, right, id), right);
	}
	AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use array as class");
		exit(1);
		//return voidClass;
	}
	inline Value* getValue(RData& r, PositionID id) const override final{
		return castToV(r,getReturnType(),id);
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		std::vector<const AbstractClass*> vec;
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec.push_back(tmp);
		}
		TupleClass* tc = TupleClass::get(vec);
		Type* t = tc->type;
		Value* v = UndefValue::get(t);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getValue(r,id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return new ConstantData(v, tc);
	}
	inline Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType!=CLASS_ARRAY
						&& right->classType!=CLASS_NAMED_TUPLE)
					id.error("Cannot cast array literal to '"+right->getName()+"'");
		ArrayClass* tc = (ArrayClass*)right;
		if(tc->len!=0){
			id.compilerError("Cannot create array[len]");
			exit(1);
		}
		uint64_t s = DataLayout(r.lmod).getTypeAllocSize(tc->inner->type);
		IntegerType* ic = llvm::IntegerType::get(getGlobalContext(), 8*sizeof(size_t));
		Instruction* v = CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
				tc->inner->type, ConstantInt::get(ic, s), ConstantInt::get(ic,inner.size()));
		r.builder.Insert(v);
		for(unsigned i = 0; i<inner.size(); i++){
			r.builder.CreateStore(inner[i]->castToV(r, tc->inner, id),
					r.builder.CreateConstGEP1_32(v, i));
		}
		assert(dyn_cast<PointerType>(tc->type));
		auto tmp=(StructType*)(((PointerType*)tc->type)->getElementType());
		s = DataLayout(r.lmod).getTypeAllocSize(tmp);
		Instruction* p = CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
						tmp, ConstantInt::get(ic, s));
		r.builder.Insert(p);
		r.builder.CreateStore(ConstantInt::get((IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));
		r.builder.CreateStore(ConstantInt::get((IntegerType*)(tmp->getElementType(1)), inner.size()),
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(ConstantInt::get((IntegerType*)(tmp->getElementType(2)), inner.size()),
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		G->getType()->dump();
				cerr << endl << flush;
		tmp->getElementType(3)->dump();
				cerr << endl << flush;
		v->getType()->dump();
				cerr << endl << flush;
		r.builder.CreateStore(v,G);
		return v;
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		if(a->classType!=CLASS_ARRAY) return false;
		ArrayClass* tc = (ArrayClass*)a;
		if(tc->len!=0 && tc->len!=inner.size()) return false;
		for(unsigned int i=0; i<inner.size(); i++){
			if(!inner[i]->hasCastValue(tc->inner)) return false;
		}
		return true;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override {
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		ArrayClass* fa = (ArrayClass*)a;
		ArrayClass* fb = (ArrayClass*)b;
		bool aBetter = false;
		bool bBetter = false;
		for(unsigned i=0; i<inner.size(); i++){
			auto j = inner[i]->compareValue(fa->inner, fb->inner);
			if(j!=0){
				if(j<0){
					if(bBetter) return 0;
					aBetter = true;
				} else {
					if(aBetter) return 0;
					bBetter = true;
				}
			}
		}
		return true;
	}

	virtual const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override{
		id.error("Cannot use array as function");
		return &VOID_DATA;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
		id.error("Tuple array act as function");
		exit(1);
	}
};



#endif /* ARRAYDATA_HPP_ */
