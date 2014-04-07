/*
 * LazyClass.hpp
 *
 *  Created on: Jan 26, 2014
 *      Author: Billy
 */

#ifndef LAZYCLASS_HPP_
#define LAZYCLASS_HPP_
#include "../AbstractClass.hpp"

#define LAZYCLASS_C_
class LazyClass: public AbstractClass{
public:
	const AbstractClass* const innerType;
	inline llvm::Type* getLazyType(const AbstractClass* const& a){
		return PointerType::getUnqual(FunctionType::get(a->type, llvm::SmallVector<Type*,0>(0),false));
	}
protected:
	LazyClass(const AbstractClass* const in):
		AbstractClass(nullptr,in->name+"%",nullptr,PRIMITIVE_LAYOUT,CLASS_LAZY,true,getLazyType(in)),
		innerType(in)
		{
		assert(in->classType!=CLASS_LAZY);
		///register methods such as print / tostring / tofile / etc
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		if(toCast->classType!=CLASS_LAZY) return false;
		else return innerType->hasCast(((LazyClass*)toCast)->innerType);
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
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override{
		illegalLocal(id,s);
		exit(1);
	}*/
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType!=CLASS_LAZY) return false;
		LazyClass* tc = (LazyClass*)toCast;
		return innerType->hasCast(tc->innerType);
	}
	inline Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		///todo
		id.compilerError("todo cast of lazy");
		exit(1);
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a->classType==CLASS_LAZY);
		assert(b->classType==CLASS_LAZY);
		LazyClass* la = (LazyClass*)a;
		LazyClass* lb = (LazyClass*)b;
		return innerType->compare(la,lb);
	}
	static LazyClass* get(const AbstractClass* const arg) {
		static std::map<const AbstractClass*,LazyClass*> map;
		LazyClass*& fc = map[arg];
		if(fc==nullptr) fc = new LazyClass(arg);
		return fc;
	}
};



#endif /* LAZYCLASS_HPP_ */
