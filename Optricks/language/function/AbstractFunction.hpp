/*
 * AbstractFunction.hpp
 *
 *  Created on: Dec 16, 2013
 *      Author: Billy
 */

#ifndef ABSTRACTFUNCTION_HPP_
#define ABSTRACTFUNCTION_HPP_
#include "../data/Data.hpp"
#include "../data/ConstantData.hpp"
#include "../FunctionProto.hpp"
#include "../RData.hpp"
#include "../evaluatable/Evaluatable.hpp"
#include "../evaluatable/CastEval.hpp"
#include "../class/AbstractClass.hpp"
/**
 * Official classification of generators:
 * 	Function whose return-type is the generator-type of its arguments
 * 	This means that generators can be mixed with non-generators...
 */

String toClassArgString(String funcName, const std::vector<const AbstractClass*>& args);
String toClassArgString(String funcName, const std::vector<const Evaluatable*>& args);

class AbstractFunction: public Data{
public:
	virtual ~AbstractFunction(){};
	const AbstractClass* getMyClass(RData& r, PositionID id) const override final{
		id.error("Cannot use function as class");
		exit(1);
	}
	const Data* toValue(RData& r, PositionID id) const override final{
		return this;
	}
	//virtual Data* callFunction(RData&,PositionID id,const std::vector<Evaluatable*>&) const =0;
	virtual FunctionProto* getFunctionProto(PositionID id) const=0;
	/**
	 * Returns null if it is not a class function
	 * Otherwise it returns the type of the callFunctioning class
	 */
	//virtual AbstractClass* isClassFunction() const=0;
	virtual llvm::Function* getValue(RData& r, PositionID id) const=0;
	const ConstantData* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	AbstractFunction():Data(R_FUNC){};
};

#define SINGLEFUNC_C_
class SingleFunction: public AbstractFunction{
	//friend OverloadedFunction;
protected:
	FunctionProto* const proto;
	llvm::Function* const myFunc;
public:
	inline FunctionProto* getSingleProto() const{
		return proto;
	}
	inline llvm::Function* getSingleFunc() const{
		assert(myFunc);
		return myFunc;
	}
	SingleFunction(FunctionProto* const fp, llvm::Function* const f):AbstractFunction(),proto(fp), myFunc(f){
		assert(fp);
		if(f)
		assert(f->getReturnType());
	};
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const{
		return proto->returnType;
	}
	Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const override final;
	const AbstractClass* getReturnType() const override final{
		return (AbstractClass*) ( proto->getFunctionClass());
	}
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	inline FunctionProto* getFunctionProto(PositionID id) const override{
		return proto;
	}
	inline llvm::Function* getValue(RData& r, PositionID id) const override{
		return myFunc;
	}
	std::vector<const Evaluatable*> validatePrototype(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const;
	static const Evaluatable* deLazy(RData& r, PositionID id, Data* val, const AbstractClass* const t) ;
	static const Evaluatable* deLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t);
	static Value* fixLazy(RData& r, PositionID id, const Data* val, const AbstractClass* const t) ;
	static Value* fixLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) ;
	static llvm::SmallVector<Value*,0> validatePrototypeNow(FunctionProto* proto, RData& r,PositionID id,const std::vector<const Evaluatable*>& args);
	Value* validatePrototypeStruct(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, Value* V) const;
};

class CompiledFunction: public SingleFunction{
private:
public:
	CompiledFunction(FunctionProto* const fp, llvm::Function* const f):SingleFunction(fp,f){
	}
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const override final;
};

llvm::Function* const createGeneratorFunction(FunctionProto* const fp, RData& r, PositionID id);
class GeneratorFunction: public SingleFunction{
public:
	GeneratorFunction(FunctionProto* const fp, RData& r, PositionID id):
		SingleFunction(fp,createGeneratorFunction(fp,r,id)){
	}
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const override final;
};
class BuiltinInlineFunction: public SingleFunction{
private:
	const std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&)> inlined;
public:
	static inline llvm::Function* getF(FunctionProto* fp);
	BuiltinInlineFunction(FunctionProto* fp, std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&)> tmp);
	BuiltinInlineFunction(FunctionProto* fp, llvm::Function* const f,std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&)> tmp):
		SingleFunction(fp,f),inlined(tmp){}
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const override final{
		return inlined(r,id,validatePrototype(r,id,args));
	}
};

#define OVERLOADEDFUNC_C_
class OverloadedFunction: public AbstractFunction{
private:
	/**
	 * Something which will generate the missing methods for a generic method
	 * null if not generic function
	 */
	void* isGeneric;
	std::vector<SingleFunction*> innerFuncs;
public:
	String const myName;
	OverloadedFunction(String name, void* generic=nullptr):AbstractFunction(),isGeneric(generic),myName(name){};
	inline void add(SingleFunction* t, PositionID id){
		assert(t);
		if(set(t,id)){
			id.error("Error overwriting function "+t->getSingleProto()->toString());
		}
	}
	inline bool set(SingleFunction* t, PositionID id){
		assert(t);
		for(unsigned int i = 0; i<innerFuncs.size(); i++){
			if(innerFuncs[i]->getSingleProto()->equals(t->getSingleProto(), id)){
				innerFuncs[i] = t;
				return true;
			}
		}
		innerFuncs.push_back(t);
		return false;
	}
	Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const override final;
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		if(innerFuncs.size()==0) return 0;
		else if(innerFuncs.size()==1) return innerFuncs[0]->compareValue(a,b);
		else return 0;//todo allow large compare value
	}
	const AbstractClass* getReturnType() const override final{
		if(innerFuncs.size()==1) return (AbstractClass*) innerFuncs[0]->getSingleProto()->getFunctionClass();
		PositionID(0,0,"#overload").compilerError("Cannot deduce return-type of overloaded function "+myName);
		exit(1);
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const{
		return getBestFit(id,args)->getSingleProto()->returnType;
	}
	SingleFunction* getBestFit(const PositionID id, const std::vector<const Evaluatable*>& args) const;
	SingleFunction* getBestFit(const PositionID id, const std::vector<const AbstractClass*>& args) const;
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const override final{
		return getBestFit(id,args)->callFunction(r,id,args);
	}
	llvm::Function* getValue(RData& r, PositionID id) const override final{
		if(innerFuncs.size()==1) return innerFuncs[0]->getSingleFunc();
		else{
			id.error("Could not get single unique function in overloaded function");
			exit(1);
		}
	}
	virtual FunctionProto* getFunctionProto(PositionID id) const override final{
		if(innerFuncs.size()==1) return innerFuncs[0]->getSingleProto();
		else{
			id.error("Could not get single unique prototype in overloaded function");
			exit(1);
		}
	}
};
#endif /* ABSTRACTFUNCTION_HPP_ */
