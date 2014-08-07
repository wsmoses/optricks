/*
 * MapData.hpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Billy
 */

#ifndef MAPDATA_HPP_
#define MAPDATA_HPP_

#include "Data.hpp"
#include "../class/builtin/HashMapClass.hpp"
#include "../class/ClassLib.hpp"
#include "../class/builtin/ClassClass.hpp"
#include "./VoidData.hpp"
#include "../../operators/Binary.hpp"
class MapData:public Data{
public:
	PositionID filePos;
	const std::vector<std::pair<const Data*,const Data*> > inner;
	MapData(PositionID id, const std::vector<std::pair<const Data*,const Data*> >& vec):Data(R_MAP),filePos(id),inner(vec){
		assert(inner.size()>0);
	};
	const AbstractClass* getReturnType() const override final{
		assert(inner.size()>0);
		const AbstractClass* A = inner[0].first->getReturnType();
		const AbstractClass* B = inner[0].second->getReturnType();
		if(A->classType==CLASS_CLASS && B->classType==CLASS_CLASS && inner.size()==1) return &classClass;
		for(unsigned i=1; i<inner.size(); i++){
			A = getMin(A, inner[i].first->getReturnType(),filePos);
			B = getMin(B, inner[i].second->getReturnType(),filePos);
		}
		return HashMapClass::get(A, B);
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_CLASS) return getMyClass(id);
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(getReturnType()==right) return this;
		return new ConstantData(castToV(r, right, id), right);
	}
	AbstractClass* getMyClass(PositionID id) const override final{
		if(inner.size()!=1)
			id.error("Cannot use map literal of size!=1 as class");
		return HashMapClass::get(inner[0].first->getMyClass(id), inner[0].second->getMyClass(id));
	}
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		return castToV(r,getReturnType(),id);
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		std::vector<std::pair<const Data*,const Data*> > vec(inner.size());
		for(unsigned int i=0; i<inner.size(); i++){
			vec[i] = std::pair<const Data*,const Data*>(inner[i].first->toValue(r, id),inner[i].second->toValue(r, id));
		}
		return new MapData(filePos, vec);
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType==CLASS_CLASS) return getMyClass(id)->getValue(r, id);
		if(right->classType!=CLASS_HASHMAP){
			id.error("Cannot cast map literal to '"+right->getName()+"'");
			exit(1);
		}
		auto tc = (HashMapClass*)right;
		assert(tc->key);
		assert(tc->value);
		auto LEN=getInt32(inner.size());//TODO CONSIDER HAVING DIF

		auto PT = llvm::PointerType::getUnqual(tc->nodeType);

		auto v = r.allocate(PT, LEN, llvm::ConstantPointerNull::get(PT));

		assert(llvm::dyn_cast<llvm::PointerType>(tc->type));
		auto tmp=(llvm::StructType*)(((llvm::PointerType*)tc->type)->getElementType());

		auto p = r.allocate(tmp);

		r.builder.CreateStore(llvm::ConstantInt::get((llvm::IntegerType*)(tmp->getElementType(0)), 0),
				r.builder.CreateConstGEP2_32(p, 0,0));
		r.builder.CreateStore(getInt32(0),
				r.builder.CreateConstGEP2_32(p, 0,1));
		r.builder.CreateStore(r.builder.CreateUDiv(r.builder.CreateMul(LEN,getInt32(4)),getInt32(3)),
				r.builder.CreateConstGEP2_32(p, 0,2));
		auto G = r.builder.CreateConstGEP2_32(p, 0,3);
		r.builder.CreateStore(v,G);

		//TODO this can be optimized with construction
		ConstantData cd(p, getReturnType());
		for(const auto& a: inner){
			TupleData td(a);
			getBinop(r, id, &cd, &td,"[]=");
		}
		return p;
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		if(a->classType==CLASS_VOID) return true;
		if(a->classType!=CLASS_HASHMAP) return false;
		auto tc = (HashMapClass*)a;
		for(unsigned int i=0; i<inner.size(); i++){
			if(!inner[i].first->hasCastValue(tc->key)) return false;
			if(!inner[i].second->hasCastValue(tc->value)) return false;
		}
		return true;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override {
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		auto fa = (HashMapClass*)a;
		auto fb = (HashMapClass*)b;
		bool aBetter = false;
		bool bBetter = false;
		for(unsigned i=0; i<inner.size(); i++){
			auto j = inner[i].first->compareValue(fa->key, fb->key);
			if(j!=0){
				if(j<0){
					if(bBetter) return 0;
					aBetter = true;
				} else {
					if(aBetter) return 0;
					bBetter = true;
				}
			}
			j = inner[i].second->compareValue(fa->value, fb->value);
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
		if(inner.size()!=1){
			id.error("Cannot use map as function");
			return &VOID_DATA;
		}
		return HashMapClass::get(inner[0].first->getMyClass(id), inner[0].second->getMyClass(id))->callFunction(r, id, args, i);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b)const override{
		if(inner.size()!=1){
			id.error("Cannot use map as function");
			exit(1);
			return nullptr;
		}
		return HashMapClass::get(inner[0].first->getMyClass(id), inner[0].second->getMyClass(id));
	}
};

#endif /* MAPDATA_HPP_ */
