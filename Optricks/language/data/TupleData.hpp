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
	TupleData(const std::vector<const Data*>& vec):Data(R_TUPLE),inner(vec){};
	const AbstractClass* getReturnType() const override final{
		std::vector<const AbstractClass*> vec;
		unsigned int i;
		for(i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			if(tmp->classType!=CLASS_CLASS) break;
			vec.push_back(tmp);
		}
		if(i==inner.size()) return &classClass;
		else{
			for(; i<inner.size(); i++){
				auto tmp = inner[i]->getReturnType();
				vec.push_back(tmp);
			}
			return TupleClass::get(vec);
		}
	}
	inline const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType==CLASS_VOID) return &VOID_DATA;
		if(right->classType!=CLASS_TUPLE && right->classType!=CLASS_NAMED_TUPLE) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		TupleClass* tc = (TupleClass*)right;
		if(tc->innerTypes.size()!=inner.size()) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		llvm::Value* v = llvm::UndefValue::get(tc->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->castToV(r,tc->innerTypes[i],id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return new ConstantData(v, tc);
	}
	inline llvm::Value* getValue(RData& r, PositionID id) const override final{
		std::vector<const AbstractClass*> vec;
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec.push_back(tmp);
		}
		auto t = TupleClass::get(vec)->type;
		llvm::Value* v = llvm::UndefValue::get(t);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getValue(r,id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return v;
	}
	const Data* toValue(RData& r,PositionID id) const override final{
		std::vector<const AbstractClass*> vec;
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getReturnType();
			vec.push_back(tmp);
		}
		TupleClass* tc = TupleClass::get(vec);
		llvm::Value* v = llvm::UndefValue::get(tc->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getValue(r,id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return new ConstantData(v, tc);
	}
	inline llvm::Value* castToV(RData& r, const AbstractClass* const right, const PositionID id) const override final{
		if(right->classType!=CLASS_TUPLE && right->classType!=CLASS_NAMED_TUPLE) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		TupleClass* tc = (TupleClass*)right;
		if(tc->innerTypes.size()!=inner.size()) id.error("Cannot cast tuple literal to '"+right->getName()+"'");
		llvm::Value* v = llvm::UndefValue::get(tc->type);
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->castToV(r,tc->innerTypes[i],id);
			v = r.builder.CreateInsertValue(v, tmp, i);
		}
		return v;
	}
	bool hasCastValue(const AbstractClass* const a) const override {
		if(a->classType==CLASS_VOID) return true;
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
	AbstractClass* getMyClass(RData& r, PositionID id, const std::vector<TemplateArg>& args) const override final{
		if(args.size()!=0){
			id.error("Cannot template non-templated class");
		}
		std::vector<const AbstractClass*> vec;
		for(unsigned int i=0; i<inner.size(); i++){
			auto tmp = inner[i]->getMyClass(r,id,{});
			vec.push_back(tmp);
		}
		return TupleClass::get(vec);
	}
};



#endif /* TUPLEDATA_HPP_ */
