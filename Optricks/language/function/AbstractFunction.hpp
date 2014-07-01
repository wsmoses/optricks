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
	const AbstractClass* getMyClass(RData& r, PositionID id) const override {
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
	virtual llvm::Constant* getValue(RData& r, PositionID id) const=0;
	const ConstantData* castTo(RData& r, const AbstractClass* const right, PositionID id) const override final;
	AbstractFunction():Data(R_FUNC){};
};

#define SINGLEFUNC_C_
class SingleFunction: public AbstractFunction{
	//friend OverloadedFunction;
protected:
	FunctionProto* const proto;
	mutable llvm::Constant* myFunc;
public:
	inline FunctionProto* getSingleProto() const{
		return proto;
	}
	virtual llvm::Constant* getSingleFunc() const{
		assert(myFunc);
		return myFunc;
	}
	SingleFunction(FunctionProto* const fp, llvm::Constant* const f):AbstractFunction(),proto(fp), myFunc(f){
		assert(fp);
		//if(f)
		//assert(f->getReturnType());
	};
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		return proto->returnType;
	}
	llvm::Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const override final;
	const AbstractClass* getReturnType() const override final{
		return (AbstractClass*) ( proto->getFunctionClass());
	}
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final;
	inline FunctionProto* getFunctionProto(PositionID id) const override{
		return proto;
	}
	inline llvm::Constant* getValue(RData& r, PositionID id) const override{
		return getSingleFunc();
	}
	std::vector<const Evaluatable*> validatePrototypeInline(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data*& instance) const;
	static const Evaluatable* deLazyInline(RData& r, PositionID id, Data* val, const AbstractClass* const t) ;
	static const Evaluatable* deLazyInline(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t);
	static llvm::Value* fixLazy(RData& r, PositionID id, const Data* val, const AbstractClass* const t) ;
	static llvm::Value* fixLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) ;
	static llvm::SmallVector<llvm::Value*,0> validatePrototypeNow(FunctionProto* proto, RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance);
	llvm::Value* validatePrototypeStruct(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance, llvm::Value* V) const;
};

class CompiledFunction: public SingleFunction{
private:
public:
	CompiledFunction(FunctionProto* const fp, llvm::Constant* const f);
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final;
};

llvm::Function* const createGeneratorFunction(FunctionProto* const fp, RData& r, PositionID id);
class GeneratorFunction: public SingleFunction{
public:
	PositionID filePos;
	GeneratorFunction(FunctionProto* const fp, PositionID id):
		SingleFunction(fp,nullptr), filePos(id){
	}
	llvm::Function* getSingleFunc() const override final;

	const AbstractClass* getMyClass(RData& r, PositionID id) const override;
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final;
};

class ExternalFunction: public SingleFunction{
private:
	String lib;
public:
	ExternalFunction(FunctionProto* const fp, String mylib=""):SingleFunction(fp,nullptr),lib(mylib){};
	llvm::Constant* getSingleFunc() const override final;
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final;
};

//CANNOT BE USED FOR LOCAL FUNCTIONS
class BuiltinInlineFunction: public SingleFunction{
private:
	const std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&,const Data*)> inlined;
public:
	static inline llvm::Function* getF(FunctionProto* fp);
	BuiltinInlineFunction(FunctionProto* fp, std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&,const Data*)> tmp):
		SingleFunction(fp,nullptr),inlined(tmp){}
	llvm::Function* getSingleFunc() const override final;
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final{
		//assert(instance==nullptr);
		return inlined(r,id,validatePrototypeInline(r,id,args,instance),instance);
	}
};

template<decltype(llvm::Intrinsic::sqrt) A>
class IntrinsicFunction: public SingleFunction{
private:
public:
	IntrinsicFunction(FunctionProto* const fp):SingleFunction(fp,nullptr){};
	llvm::Function* getSingleFunc() const override final;
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final;
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
	llvm::Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const override final;
	bool hasCastValue(const AbstractClass* const a) const override final;
	int compareValue(const AbstractClass* const a, const AbstractClass* const b) const override final{
		if(innerFuncs.size()==0) return 0;
		else if(innerFuncs.size()==1) return innerFuncs[0]->compareValue(a,b);
		else return 0;//todo allow large compare value
	}
	const AbstractClass* getReturnType() const override final{
		if(innerFuncs.size()==1) return (AbstractClass*) innerFuncs[0]->getSingleProto()->getFunctionClass();
		PositionID(0,0,"#overload").compilerError("Cannot deduce return-type of overloaded function "+myName+" "+str(innerFuncs.size()));
		exit(1);
	}
	//TODO ALLOW TEMPLATE ARGS
	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const{
		return getBestFit(id,args, isClassMethod)->getSingleProto()->returnType;
	}
	SingleFunction* getBestFit(const PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod) const;
	SingleFunction* getBestFit(const PositionID id, const std::vector<const AbstractClass*>& args, bool isClassMethod) const;

	//TODO ALLOW TEMPLATE ARGS
	const Data* callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const override final{
		return getBestFit(id,args, instance!=nullptr)->callFunction(r,id,args, instance);
	}
	llvm::Constant* getValue(RData& r, PositionID id) const override final{
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
