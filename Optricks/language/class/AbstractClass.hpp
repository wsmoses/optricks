/*
 * AbstractClass.hpp
 *
 *  Created on: Dec 23, 2013
 *      Author: Billy
 */

#ifndef ABSTRACTCLASS_HPP_
#define ABSTRACTCLASS_HPP_
#include "../includes.hpp"
#include "../module/Scopable.hpp"
#include "../data/literal/Literal.hpp"
#include "../function/AbstractFunction.hpp"

#define ABSTRACTCLASS_C_
class AbstractClass: public Literal{
	friend Scopable;
	friend ReferenceClass;
	friend LazyClass;
	friend Lexer;
public:
	const Scopable* const myScope;
	mutable Scopable staticVariables;
	virtual ~AbstractClass(){};
private:
	const String name;
public:
	const AbstractClass* const superClass;
	const LayoutType layout;
	const ClassType classType;
	const bool isFinal;
	llvm::Type* const type;
protected:
public:
	//virtual bool hasCast(AbstractClass* cl) const=0;
	/**
	 * Casting requires no operation
	 */
	virtual bool noopCast(const AbstractClass* const toCast) const=0;
	virtual bool hasCast(const AbstractClass* const toCast) const=0;
	//virtual std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s)=0;
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	virtual llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const=0;

	//virtual const Data* applyPreop(RData& r, const PositionID id, const Data* toApply, String op) const=0;
	//virtual const Data* applyPostop(RData& r, const PositionID id, const Data* toApply, String op) const=0;
	//binary operators will be handled somewhere else --
	//in one big class that handles all binary  operators
//	const String name; this really shouldn't be here...
	inline bool hasSuper(const AbstractClass* const a) const{
		assert(layout==POINTER_LAYOUT || layout==PRIMITIVEPOINTER_LAYOUT);
		const AbstractClass* up = superClass;
		while(up!=NULL){
			if(up==a) return true;
			else up=up->superClass;
		}
		return false;
	}

	String getName() const{
		if(myScope==nullptr) return name;
		else{
			auto s = myScope->getScopeName();
			if(s.length()==0) return name;
			return s + "." + name;
		}
	}
	inline void illegalCast(PositionID id, const AbstractClass* const right) const{
		id.error("Cannot cast class '"+getName()+"' to '"+right->getName()+"'");
	}
	inline void illegalLocal(PositionID id, String s) const{
		id.error("Cannot find local variable '"+s+"' in class '"+getName()+"'");
	}
	const Token getToken() const override{ return T_ABSTRACTCLASS; };
	const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	AbstractClass* getSelfClass(PositionID id) override final{
		return this;
	}
	const AbstractClass* getReturnType() const override;
	inline llvm::Constant* getValue(RData& r, PositionID id) const override final{
		return llvm::ConstantInt::get(CLASSTYPE, (uint64_t)this, false);
	}
	bool hasCastValue(const AbstractClass* const a) const override final{
		return a->classType==CLASS_CLASS || a->classType==CLASS_VOID;
	}
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(a);
		assert(b);
		assert(hasCast(a));
		assert(hasCast(b));
		return 0;
	}
	llvm::Constant* castToV(RData& r, const AbstractClass* const right, PositionID id) const override final{
		if(right->classType!=CLASS_CLASS) id.error("Cannot cast class 'class' to '"+right->getName()+"'");
		return getValue(r,id);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
		return this;
	}
	/**
	 * returns -1 if a is easier to cast to than b
	 * +1 if b is easier to cast to than b
	 * 0 if a is equally hard to cast to as b
	 * PRECONDITION
	 * 	hasCast(a)==true
	 * 	hasCast(b)==true
	 * compare(a,a)==0
	 * compare(a,b) == - compare(b,a)
	 */
	virtual int compare(const AbstractClass* const a, const AbstractClass* const b) const=0;

	virtual const AbstractClass* getLocalReturnClass(PositionID id, String s) const=0;
	virtual bool hasLocalData(String s) const=0;
	virtual const Data* getLocalData(RData& r,PositionID id, String s, const Data* instance) const=0;

	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		return this;
	}
	const Data* callFunction(RData& r, PositionID id, const std::vector<const Evaluatable*>& args) const override final;
	inline AbstractClass(const Scopable* const sc, const String nam, const AbstractClass* const supa, LayoutType const t, ClassType const ct, bool const fina, llvm::Type* const tp=nullptr, Scopable* ss=nullptr)
	:
		Literal(R_CLASS),myScope(sc),
		staticVariables(ss?ss:(supa?(&(supa->staticVariables)):nullptr), nam),
		name(nam),
		superClass(supa),
		layout(t),
		classType(ct),
		isFinal(fina)
	,type(tp)
	{
		//cerr << getName() << "|" << (ss?(ss->getScopeName()):"null") << endl << flush;
	};
};



#endif /* ABSTRACTCLASS_HPP_ */
