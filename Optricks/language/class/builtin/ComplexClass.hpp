/*
 * ComplexClass.hpp
 *
 *  Created on: Jan 2, 2014
 *      Author: Billy
 */

#ifndef COMPLEXCLASS_HPP_
#define COMPLEXCLASS_HPP_

#include "../AbstractClass.hpp"
class ComplexClass: public AbstractClass{
public:
	const RealClass* innerClass;
	static llvm::Type* cType(const RealClass* inner){
		assert(inner);
		if(!(inner->type->isFloatingPointTy() || inner->type->isIntegerTy())){
			inner->type->dump();
			cerr << endl << flush;
		}
		assert(inner->type->isFloatingPointTy() || inner->type->isIntegerTy());
		return inner->type;
	}
	ComplexClass(String name, const RealClass* inner, bool reg = false);
	inline llvm::Constant* getValue(PositionID id, const mpfr_t& value) const{
		if(innerClass->classType!=CLASS_FLOAT) id.error("Cannot convert floating literal to "+getName());
		const FloatClass* in = (const FloatClass*)innerClass;
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		ar[0] = in->getValue(id,value);
		ar[1] = innerClass->getZero(id);
		return llvm::ConstantVector::get(ar);
	}
	bool hasLocalData(String s) const override final{
		return s=="real" || s=="imag";
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		if(s!="real" && s!="imag"){
			illegalLocal(id,s);
			exit(1);
		} else return innerClass;
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final;
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		assert(innerClass);
		return (toCast->classType==CLASS_COMPLEX && innerClass->noopCast(((ComplexClass*)toCast)->innerClass))|| toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		assert(innerClass);
		return (toCast->classType==CLASS_COMPLEX && innerClass->hasCast(((ComplexClass*)toCast)->innerClass))|| toCast->classType==CLASS_VOID;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return innerClass->compare(((ComplexClass*)a)->innerClass, ((ComplexClass*)b)->innerClass);
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType!=CLASS_COMPLEX) id.error("Cannot cast "+getName()+" type to "+toCast->getName());
		//if(toCast==this || innerClass->noopCast(((ComplexClass*)toCast)->innerClass)) return valueToCast;
		//if(!innerClass->hasCast(((ComplexClass*)toCast)->innerClass)) id.error()
		auto V = innerClass->castTo(((ComplexClass*)toCast)->innerClass, r, id, valueToCast);
		assert(V->getType()==toCast->type);
		return V;
	}
	static inline ComplexClass* get(const RealClass* inner) {
		static std::map<const RealClass*, ComplexClass*> cache;
		auto found = cache.find(inner);
		if(found==cache.end()){
			assert(inner);
			auto a="complex{"+inner->getName()+"}";
			ComplexClass* nex = new ComplexClass(a,inner);
			cache.insert(std::pair<const RealClass*,ComplexClass*>(inner, nex));
			return nex;
		}
		else return found->second;
	}
	inline llvm::Constant* getZero(PositionID id, bool negative=false) const{
		return llvm::ConstantVector::getSplat(2, innerClass->getZero(id,negative));
	}
	inline llvm::Constant* getOne(PositionID id) const{
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		ar[0] = innerClass->getOne(id);
		ar[1] = innerClass->getZero(id);
		return llvm::ConstantVector::get(ar);
	}
	inline llvm::Constant* getI(PositionID id) const{
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		ar[0] = innerClass->getZero(id);
		ar[1] = innerClass->getOne(id);
		return llvm::ConstantVector::get(ar);
	}
	inline llvm::Constant* getValue(PositionID id, const mpz_t& value) const{
		llvm::SmallVector<llvm::Constant*,2> ar(2);
		ar[0] = innerClass->getValue(id,value);
		ar[1] = innerClass->getZero(id);
		return llvm::ConstantVector::get(ar);
	}
};

//const ComplexClass* complexClass("complex",&doubleClass, true);

#endif /* COMPLEXCLASS_HPP_ */
