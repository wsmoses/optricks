/*
 * TupleData.hpp
 *
 *  Created on: Jan 22, 2014
 *      Author: Billy
 */

#ifndef TUPLEDATA_HPP_
#define TUPLEDATA_HPP_

#include "Data.hpp"
#include "../class/builtin/TupleClass.hpp"
#include "../class/builtin/ClassClass.hpp"
#include "./VoidData.hpp"
class TupleData:public Data{
public:
	const std::vector<const Data*> inner;
	TupleData(const std::pair<const Data*,const Data*>& vec):Data(R_TUPLE),inner({vec.first,vec.second}){
	};
	TupleData(const std::vector<const Data*>& vec):Data(R_TUPLE),inner(vec){
	};
	TupleData(bool b):Data(R_TUPLE){
	};
	const AbstractClass* getReturnType() const override final{
		std::vector<const AbstractClass*> vec(inner.size());
		unsigned int i;
		for(i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec[i] = tmp;
			if(tmp->classType!=CLASS_CLASS) break;
		}
		if(i==inner.size()) return &classClass;
		else{
			i++;
			for(; i<inner.size(); i++){
				vec[i] = inner[i]->getReturnType();
			}
			assert(vec.size()==inner.size());
			return TupleClass::get(vec);
		}
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(right->classType==CLASS_CLASS) return getMyClass(id);
		if(right->classType!=CLASS_TUPLE && right->classType!=CLASS_NAMED_TUPLE) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		TupleClass* tc = (TupleClass*)right;
		if(tc->innerTypes.size()!=inner.size()) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		llvm::Value* v = getUndef(tc->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->castToV(r,tc->innerTypes[i],id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return new ConstantData(v, tc);
	}
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		std::vector<const AbstractClass*> vec(inner.size());
		unsigned int i;
		for(i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec[i] = tmp;
			if(tmp->classType!=CLASS_CLASS) break;
		}
		if(i==inner.size()){
			return getMyClass(id)->getValue(r, id);
		} else {
			i++;
			for(; i<inner.size(); i++){
				vec[i] = inner[i]->getReturnType();
			}
		}
		assert(vec.size()==inner.size());
		auto t = TupleClass::get(vec);
		llvm::Value* v = getUndef(t->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getValue(r,id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return v;
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		std::vector<const AbstractClass*> vec(inner.size());
		unsigned int i;
		for(i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec[i] = tmp;
			if(tmp->classType!=CLASS_CLASS) break;
		}
		if(i==inner.size()){
			return getMyClass(id);
		} else {
			i++;
			for(; i<inner.size(); i++){
				vec[i] = inner[i]->getReturnType();
			}
		}
		assert(vec.size()==inner.size());
		auto t = TupleClass::get(vec);
		llvm::Value* v = getUndef(t->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getValue(r,id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return new ConstantData(v, t);
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType==CLASS_CLASS) return getMyClass(id)->getValue(r, id);
		if(right->classType!=CLASS_TUPLE && right->classType!=CLASS_NAMED_TUPLE) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		TupleClass* tc = (TupleClass*)right;
		if(tc->innerTypes.size()!=inner.size()) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		llvm::Value* v = getUndef(tc->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->castToV(r,tc->innerTypes[i],id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return v;
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		if(a->classType==CLASS_VOID) return true;
		if(a->classType==CLASS_CLASS){
			for(unsigned int i=0; i<inner.size(); i++){
				if(!inner[i]->hasCastValue(a)) return false;
			}
			return true;
		}
		if(a->classType!=CLASS_TUPLE && a->classType!=CLASS_NAMED_TUPLE) return false;
		TupleClass* tc = (TupleClass*)a;
		if(tc->innerTypes.size()!=inner.size()) return false;
		for(unsigned int i=0; i<inner.size(); i++){
			if(!inner[i]->hasCastValue(tc->innerTypes[i])) return false;
		}
		return true;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override {
		assert(hasCastValue(a));
		assert(hasCastValue(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		TupleClass* fa = (TupleClass*)a;
		TupleClass* fb = (TupleClass*)b;
		assert(fa->innerTypes.size() == inner.size());
		assert(fb->innerTypes.size() == inner.size());
		bool aBetter = false;
		bool bBetter = false;
		if(a->classType==CLASS_NAMED_TUPLE || b->classType==CLASS_NAMED_TUPLE) return 0;
		for(unsigned i=0; i<inner.size(); i++){
			auto j = inner[i]->compareValue(fa->innerTypes[i], fb->innerTypes[i]);
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

	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args, const Data* inst) const override{
		id.error("Cannot use tuple as function");
		return &VOID_DATA;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool b)const override{
		id.error("Tuple cannot act as function");
		exit(1);
	}
	/**
	 * Returns the class that this represents, if it represents a class
	 */
	AbstractClass* getMyClass(PositionID id) const override final{
		std::vector<const AbstractClass*> vec(inner.size());
		for(unsigned int i=0; i<inner.size(); i++){
			vec[i] = inner[i]->getMyClass(id);
		}
		assert(vec.size()==inner.size());
		return TupleClass::get(vec);
	}
};



#endif /* TUPLEDATA_HPP_ */
