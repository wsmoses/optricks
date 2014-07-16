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
		String s= "function{"+r->getName();
		for(const auto& b: a) s+=", "+b->getName();
		if(va)s+=", ...";
		return s+"}";
	}
	static inline llvm::Type* getFuncType(const AbstractClass* const r1, const std::vector<const AbstractClass*>& args, bool isVarArg){
		const auto len = args.size();
		llvm::SmallVector<llvm::Type*,0> ar(len);
		for(unsigned int i=0; i<len; i++){
			ar[i]=args[i]->type;
			assert(ar[i]);
		}
		return llvm::PointerType::getUnqual(llvm::FunctionType::get(r1->type,ar,isVarArg));
	}
	const AbstractClass* const returnType;
	std::vector<const AbstractClass*> argumentTypes;
	bool isVarArg;
private:
	FunctionClass(const AbstractClass* const r1, const std::vector<const AbstractClass*>& args, bool var=false):
		AbstractClass(nullptr,str(r1,args,var),nullptr,PRIMITIVE_LAYOUT,CLASS_FUNC,true,getFuncType(r1,args,var)),returnType(r1),argumentTypes(args),isVarArg(var){

		LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
			new BuiltinInlineFunction(
					new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			r.printf(this->getName()+"(0x%" PRIXPTR ")", args[0]->evalV(r, id));
			return &VOID_DATA;
		}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#str"),"println")->add(
			new BuiltinInlineFunction(
					new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			r.printf(this->getName()+"(0x%" PRIXPTR ")\n", args[0]->evalV(r, id));
			return &VOID_DATA;
		}), PositionID(0,0,"#int"));
		//cerr << getName() << " is ";
		//type->dump();
		//cerr << endl << flush;
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_CPOINTER: return true;
		case CLASS_FUNC: {
			return noopCast(toCast);
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		illegalLocal(id,s);
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	inline bool noopCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		if(toCast->classType==CLASS_CPOINTER) return true;
		if(toCast->classType!=CLASS_FUNC){
			//cerr << getName() << "is not func / " << toCast->getName() << endl << flush;
			return false;
		}
		const FunctionClass* const fc = (FunctionClass*)toCast;
		if(fc->returnType->classType!=CLASS_VOID && !returnType->noopCast(fc->returnType)){
			//cerr << "RET: "<< returnType->getName() << " cannot cast to " << fc->returnType->getName() << endl << flush;
			return false;
		}
		const auto a1=argumentTypes.size();
		const auto a2 = fc->argumentTypes.size();
		if(a1!=a2){
			//cerr << "ARGLEN: "<< a1 << " != " << a2 << endl << flush;
			return false;
		}
		for(unsigned i=0; i<a1; i++)
			if(!argumentTypes[i]->noopCast(fc->argumentTypes[i])){
			//	cerr << "ARG " << i << ": " << argumentTypes[i]->getName() << " cannot cast to " << fc->argumentTypes[i]->getName() << endl << flush;
				return false;
			}

		//cerr << getName() << " CAN CAST TO " << toCast->getName() << endl << flush;
		return true;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	inline llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const;
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		if(a==this && b==this) return 0;
		else if(a==this) return -1;
		else if(b==this) return 1;
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
		if(fc==nullptr) fc= new FunctionClass(ret, args);
		//cerr << FunctionClass::str(ret, args,false) << " VS " << fc->getName() << endl << flush;
		return fc;
	}
};



#endif /* FUNCTIONCLASS_HPP_ */
