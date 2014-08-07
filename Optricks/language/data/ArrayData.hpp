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
		for(unsigned i=1; i<inner.size(); i++){
			A = getMin(A, inner[i]->getReturnType(),filePos);
		}
		return ArrayClass::get(A);
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(getReturnType()==right) return this;
		return new ConstantData(castToV(r, right, id), right);
	}
	AbstractClass* getMyClass(PositionID id) const override final{
		id.error("Cannot use array as class");
		exit(1);
		//return voidClass;
	}
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		return castToV(r,getReturnType(),id);
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		std::vector<const Data*> vec(inner.size());
		for(unsigned int i=0; i<inner.size(); i++){
			vec[i] = inner[i]->toValue(r, id);
		}
		return new ArrayData(vec, filePos);
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType!=CLASS_ARRAY){
			id.error("Cannot cast array literal to '"+right->getName()+"'");
			exit(1);
		}
		ArrayClass* tc = (ArrayClass*)right;
		//TODO have an "empty" slot
		auto v = r.allocate(tc->inner->type, getSizeT(inner.size()));

		for(unsigned i = 0; i<inner.size(); i++){
			r.builder.CreateStore(inner[i]->castToV(r, tc->inner, id),
					r.builder.CreateConstGEP1_32(v, i));
		}
		assert(llvm::dyn_cast<llvm::PointerType>(tc->type));
		auto tmp=(llvm::StructType*)(((llvm::PointerType*)tc->type)->getElementType());

		auto p=r.allocate(tmp);

		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));
		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(1)), inner.size()),
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(2)), inner.size()),
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		r.builder.CreateStore(v,G);
		return p;
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		if(a->classType==CLASS_VOID) return true;
		if(a->classType!=CLASS_ARRAY) return false;
		ArrayClass* tc = (ArrayClass*)a;
		//if(tc->len!=0 && tc->len!=inner.size()) return false;
		for(unsigned int i=0; i<inner.size(); i++){
			if(!inner[i]->hasCastValue(tc->inner)) return false;
		}
		return true;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override {
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
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

	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* i) const override{
		id.error("Cannot use array as function");
		return &VOID_DATA;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b)const override{
		id.error("Cannot use array as function");
		exit(1);
	}
};

#endif /* ARRAYDATA_HPP_ */
