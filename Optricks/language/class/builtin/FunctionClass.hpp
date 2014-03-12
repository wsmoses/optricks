/*
 * FunctionClass.hpp
 *
 *  Created on: Dec 30, 2013
 *      Author: Billy
 */

#ifndef FUNCTIONCLASS_HPP_
#define FUNCTIONCLASS_HPP_
#include "../AbstractClass.hpp"

#define FUNCCLASS_C_
class FunctionClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const r, const std::vector<const AbstractClass*>& a, bool va){
		String s= "function<"+r->getName();
		for(const auto& b: a) s+=", "+b->getName();
		if(va)s+=", ...";
		return s+">";
	}
	static inline Type* getFuncType(const AbstractClass* const r1, const std::vector<const AbstractClass*>& args, bool isVarArg){
				const auto len = args.size();
				llvm:SmallVector<Type*,0> ar(len);
				for(unsigned int i=0; i<len; i++)ar[i]=args[i]->type;
				return FunctionType::get(r1->type,ar,isVarArg);
	}
	const AbstractClass* const returnType;
	std::vector<const AbstractClass*> argumentTypes;
	bool isVarArg;
private:
	FunctionClass(const AbstractClass* const r1, const std::vector<const AbstractClass*>& args, bool var=false):
		AbstractClass(nullptr,str(r1,args,var),nullptr,PRIMITIVE_LAYOUT,CLASS_FUNC,true,getFuncType(r1,args,var)),returnType(r1),argumentTypes(args),isVarArg(var){}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_CPOINTER: return true;
		case CLASS_FUNC: {
			return noopCast(toCast);
	/*
			if(toCast==this) return true;
			//TODO
			assert(0 & "Function class autocasting has yet to be determined in the Optricks spec");
			cerr << "Function class autocasting has yet to be determined in the Optricks spec" << endl << flush;
			exit(1);*/
		}
		default:
			return false;
		}
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		illegalLocal(id,s);
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType!=CLASS_FUNC) return false;
		const FunctionClass* const fc = (FunctionClass*)toCast;
		if(!returnType->noopCast(fc->returnType)) return false;
		const auto a1=argumentTypes.size();
		const auto a2 = fc->argumentTypes.size();
		if(a1!=a2) return false;
		for(unsigned i=0; i<a1; i++)
			if(!argumentTypes[i]->noopCast(fc->argumentTypes[i])) return false;
		return true;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	inline Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const;
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_FUNC || a->classType==CLASS_CPOINTER);
		assert(b->classType==CLASS_FUNC || b->classType==CLASS_CPOINTER);
		if(a->classType==CLASS_CPOINTER)
			return (b->classType==CLASS_CPOINTER)?(0):(1);
		else if(b->classType==CLASS_CPOINTER)
			return -1;
		FunctionClass* fa = (FunctionClass*)a;
		FunctionClass* fb = (FunctionClass*)b;
		assert(fa->argumentTypes.size() == argumentTypes.size());
		assert(fb->argumentTypes.size() == argumentTypes.size());
		bool aBetter = false;
		bool bBetter = false;
		for(unsigned i=0; i<argumentTypes.size(); i++){
			auto j = argumentTypes[i]->compare(fa->argumentTypes[i], fb->argumentTypes[i]);
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
		if(aBetter) return -1;
		else if(bBetter) return 1;
		else return 0;
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	static FunctionClass* get(const AbstractClass* const ret, std::vector<const AbstractClass*>& args) {
		static Mapper<const AbstractClass*, FunctionClass*> map;
		args.push_back(ret);
		FunctionClass*& fc = map.get(args);
		args.pop_back();
		if(fc==nullptr) return fc= new FunctionClass(ret, args);
		return fc;
	}
};



#endif /* FUNCTIONCLASS_HPP_ */
