/*
 * AbstractFunctionP.hpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Billy
 */

#ifndef ABSTRACTFUNCTIONP_HPP_
#define ABSTRACTFUNCTIONP_HPP_
#include "AbstractFunction.hpp"
#include "../class/builtin/FunctionClass.hpp"
#include "../class/builtin/LazyClass.hpp"
#include "../class/builtin/ReferenceClass.hpp"


BuiltinInlineFunction::BuiltinInlineFunction(FunctionProto* fp, std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&)> tmp):
SingleFunction(fp,getF(fp)),inlined(tmp){
	BasicBlock *Parent = rdata.builder.GetInsertBlock();
	BasicBlock *BB = rdata.CreateBlockD("entry", myFunc);
	rdata.builder.SetInsertPoint(BB);

	unsigned Idx = 0;
	std::vector<const Evaluatable*> args;
	for (Function::arg_iterator AI = myFunc->arg_begin(); Idx != myFunc->arg_size();
			++AI, ++Idx) {
		((Value*)AI)->setName(Twine(proto->declarations[Idx].declarationVariable));
		//todo should have this be location?
		if(proto->declarations[Idx].declarationType->classType==CLASS_REF)
			args.push_back(new LocationData(new StandardLocation(AI),proto->declarations[Idx].declarationType));
		else
			args.push_back(new ConstantData(AI,proto->declarations[Idx].declarationType));
	}
	const Data* ret = inlined(rdata, PositionID(0,0,"#inliner"), args);
	if(! rdata.hadBreak()){
		if(proto->returnType->classType==CLASS_VOID)
			rdata.builder.CreateRetVoid();
		else
			rdata.builder.CreateRet(ret->getValue(rdata,PositionID(0,0,"#inliner")));
	}
	rdata.FinalizeFunctionD(myFunc);
	if(Parent) rdata.builder.SetInsertPoint( Parent );
}

inline llvm::Function* BuiltinInlineFunction::getF(FunctionProto* fp){
	auto tmp=fp->declarations.size();
	Type* ar[tmp];
	for(unsigned i=0; i<tmp; i++)
		ar[i] = fp->declarations[i].declarationType->type;
	llvm::FunctionType* FT = FunctionType::get(fp->returnType->type, ArrayRef<Type*>(ar, tmp), false);
	return rdata.CreateFunctionD(fp->name, FT, LOCAL_FUNC);
}
String toClassArgString(String funcName, const std::vector<const AbstractClass*>& args){
	String s=funcName+"(";
	bool first=true;
	for(const auto& a:args){
		if(first) first=!first;
		else s+=", ";
		s+=a->getName();
	}
	return s+")";
}
String toClassArgString(String funcName, const std::vector<const Evaluatable*>& args){
	String s=funcName+"(";
	bool first=true;
	for(const auto& a:args){
		if(first) first=!first;
		else s+=", ";
		s+=a->getReturnType()->getName();
	}
	return s+")";
}


std::vector<const Evaluatable*> SingleFunction::validatePrototype(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const {
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	if(as>ds) id.error("Gave too many arguments to function "+proto->toString());
	std::vector<const Evaluatable*> arg2;
	for(unsigned int i = 0; i<as; i++){
		const AbstractClass* const t = proto->declarations[i].declarationType;
		const AbstractClass* const at = args[i]->getReturnType();
		if(t->classType==CLASS_LAZY) id.error("Cannot use lazy arguments on regular function");

		else if(at->classType==CLASS_VOID){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}

			arg2.push_back(deLazy(r,id,proto->declarations[i].defaultValue,t));
		}
		else
			arg2.push_back(deLazy(r,id,args[i],t));
		assert(arg2.back() != NULL);
	}
	for(unsigned int i = as; i<ds; i++){
		if(proto->declarations[i].defaultValue==nullptr){
			id.error("No default argument available for argument "+str(i+1));
			exit(1);
		}
		const AbstractClass* const t = proto->declarations[i].declarationType;
		arg2.push_back(deLazy(r,id,proto->declarations[i].defaultValue,t));
	}
	return arg2;
}
const Evaluatable* SingleFunction::deLazy(RData& r, PositionID id, Data* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		if(t==val->getReturnType()) return val;
		else return new CastEval(val,t,id);
	} else if(t->classType==CLASS_REF){
		if(val->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = val->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return val;
	} else {
		return val->castTo(r, t, id);
	}
}
const Evaluatable* SingleFunction::deLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		if(t==val->getReturnType()) return val;
		else return new CastEval(val,t,id);
	} else if(t->classType==CLASS_REF){
		const Data* tt = val->evaluate(r);
		if(tt->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = tt->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return tt;
	} else {
		return new ConstantData(val->evaluate(r)->castToV(r, t, id),t);
	}
}
Value* SingleFunction::fixLazy(RData& r, PositionID id, const Data* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		const LazyClass* const lc = (const LazyClass*)t;
		FunctionType *FT = (llvm::FunctionType*)(lc->type);
		Function* F = Function::Create(FT,LOCAL_FUNC,"%lazy",& r.lmod);
		BasicBlock *Parent = r.builder.GetInsertBlock();
		BasicBlock *BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		r.builder.CreateRet(val->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
		return F;
	} else if(t->classType==CLASS_REF){
		if(val->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = val->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return ((LocationData*)val)->value->getPointer(r, id);
	} else {
		return val->castToV(r, t, id);
	}
}
Value* SingleFunction::fixLazy(RData& r, PositionID id, Evaluatable* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		const LazyClass* lc = (const LazyClass*)t;
		FunctionType *FT = (llvm::FunctionType*)(lc->type);
		Function* F = Function::Create(FT,LOCAL_FUNC,"%lazy",& r.lmod);
		BasicBlock *Parent = r.builder.GetInsertBlock();
		BasicBlock *BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		r.builder.CreateRet(val->evaluate(r)->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
		return F;
	} else if(t->classType==CLASS_REF){
		const Data* tt = val->evaluate(r);
		if(tt->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = tt->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return ((LocationData*)tt)->value->getPointer(r, id);
	} else {
		return val->evaluate(r)->castToV(r, t, id);
	}
}
ArrayRef<Value*> SingleFunction::validatePrototypeNow(FunctionProto* proto, RData& r,PositionID id,const std::vector<const Evaluatable*>& args){
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	if(as>ds) id.error("Gave too many arguments to function "+proto->toString());
	Value* temp[ds];
	for(unsigned int i = 0; i<as; i++){
		const AbstractClass* const t = proto->declarations[i].declarationType;
		const AbstractClass* const at = args[i]->getReturnType();
		if(!at) id.error("What...")
		assert(at);
		if(at->classType==CLASS_VOID){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}
			temp[i] = fixLazy(r, id, proto->declarations[i].defaultValue, t);
		}
		else{
			temp[i] = fixLazy(r, id, args[i]->evaluate(r), t);
		}
		assert(temp[i] != NULL);
	}
	for(unsigned int i = as; i<ds; i++){
		if(proto->declarations[i].defaultValue==nullptr){
			id.error("No default argument available for argument "+str(i+1));
			exit(1);
		}
		const AbstractClass* const t = proto->declarations[i].declarationType;
		temp[i] = fixLazy(r, id, proto->declarations[i].defaultValue->evaluate(r), t);
	}
	return ArrayRef<Value*>(temp, ds);
}
Value* SingleFunction::validatePrototypeStruct(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, Value* V) const{
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	if(as>ds) id.error("Gave too many arguments to function "+proto->toString());
	for(unsigned int i = 0; i<as; i++){
		const AbstractClass* const t = proto->declarations[i].declarationType;
		const AbstractClass* const at = args[i]->getReturnType();
		Value* temp;
		if(at->classType==CLASS_VOID){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}
			temp = fixLazy(r, id, proto->declarations[i].defaultValue, t);
		}
		else{
			temp = fixLazy(r, id, args[i]->evaluate(r), t);
		}
		V = r.builder.CreateInsertValue(V, temp, i);
		assert(V != NULL);
	}
	for(unsigned int i = as; i<ds; i++){
		if(proto->declarations[i].defaultValue==nullptr){
			id.error("Error: No default argument available for argument "+str(i+1));
			exit(1);
		}
		const AbstractClass* const t = proto->declarations[i].declarationType;
		V = r.builder.CreateInsertValue(V, fixLazy(r, id, proto->declarations[i].defaultValue->evaluate(r), t), i);
	}
	return V;
}


bool SingleFunction::hasCastValue(const AbstractClass* const a)const  {
	return proto->getFunctionClass()->hasCast(a);
}
int SingleFunction::compareValue(const AbstractClass* const a, const AbstractClass* const b) const {
	return proto->getFunctionClass()->compare(a,b);
}

const ConstantData* AbstractFunction::castTo(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
	case CLASS_FUNC:
		id.error("Function classes have not been implemented...");
		exit(1);
	case CLASS_CPOINTER:
		return new ConstantData(r.builder.CreatePointerCast(getValue(r, id),C_POINTERTYPE), right);
	default:
		id.error("Cannot cast function to non-function class "+right->getName());
		exit(1);
	}
}

Value* SingleFunction::castToV(RData& r, const AbstractClass* const right, PositionID id) const{
	switch(right->classType){
	case CLASS_FUNC:{
		auto fc= proto->getFunctionClass();
		if(fc->noopCast(right)) return myFunc;
		else {
			id.error("Single Function automatic generation of types has not been implemented...");
			exit(1);
		}
	}
	case CLASS_CPOINTER:
		return r.builder.CreatePointerCast(getValue(r, id),C_POINTERTYPE);
	default:
		id.error("Cannot cast function to non-function class "+right->getName());
		exit(1);
	}
}

llvm::Function* const createGeneratorFunction(FunctionProto* const fp, RData& r, PositionID id){
	std::vector<Type*> args;

	for(auto& a: fp->declarations) args.push_back(a.declarationType->type);
	auto gt = fp->getGeneratorType()->type;
	FunctionType *FT = FunctionType::get(gt, ArrayRef<Type*>(args), false);
	Function* F = r.CreateFunctionD(fp->name,FT,LOCAL_FUNC);
	BasicBlock *Parent = r.builder.GetInsertBlock();
	BasicBlock *BB = BasicBlock::Create(r.lmod.getContext(), "entry", F);
	r.builder.SetInsertPoint(BB);
	assert(dyn_cast<VectorType>(gt)!=nullptr);
	Value *V = UndefValue::get(gt);
	auto const tmp=fp->declarations.size();
	unsigned Idx = 0;
	for (Function::arg_iterator AI = F->arg_begin(); Idx != tmp;
			++AI, ++Idx)
		V = r.builder.CreateInsertValue(V, (Value*)AI, ArrayRef<unsigned int>({Idx}));
	r.builder.CreateRet(V);
	if(Parent) r.builder.SetInsertPoint(Parent);
	return F;
}

const Data* GeneratorFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const{
	auto gt=proto->getGeneratorType();
	Value *V = UndefValue::get(gt->type);
	return new ConstantData(validatePrototypeStruct(r,id,args,V),gt);
}


Value* OverloadedFunction::castToV(RData& r, const AbstractClass* const right, PositionID id) const {
	switch(right->classType){
	case CLASS_FUNC:{
		//todo .. have cast (no-op) wrapper on this
		FunctionClass* fc = (FunctionClass*)right;
		return getBestFit(id, fc->argumentTypes)->myFunc;
	}
	case CLASS_CPOINTER:
		return r.builder.CreatePointerCast(getValue(r, id),C_POINTERTYPE);
	default:
		id.error("Cannot cast function to non-function class "+right->getName());
		exit(1);
	}
}

bool OverloadedFunction::hasCastValue(const AbstractClass* const a) const {
	if(a->classType==CLASS_CPOINTER) return true;
	if(a->classType!=CLASS_FUNC) return false;
	if(isGeneric) return true;
	auto args = ((FunctionClass*)a)->argumentTypes;
	std::list<SingleFunction*> choices;
	for(auto& a: innerFuncs){
		if(a->proto->declarations.size()>=args.size()){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(args[i]->classType==CLASS_VOID){
					if(a->proto->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					}
				} else if(!args[i]->hasCast(a->proto->declarations[i].declarationType)){
					valid=false;
					break;
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->proto->declarations.size(); i++)
				if(a->proto->declarations[i].defaultValue==nullptr){
					valid=false;
					break;
				}
			if(!valid) continue;
			choices.push_back(a);
		}
	}
	if(choices.size()==0) return false;
	for(unsigned int i=0; i<args.size() && choices.size()>1; i++){
		std::list<SingleFunction*>::iterator best=choices.begin();
		std::list<SingleFunction*>::iterator current=choices.begin();
		++current;
		for(; current!=choices.end();){
			//less means better
			auto c=args[i]->compare((*best)->proto->declarations[i].declarationType, (*current)->proto->declarations[i].declarationType);
			if(c==0){
				++current;
			} else if(c<0){
				auto tmp=current;
				++current;
				choices.erase(tmp);
			} else{
				choices.erase(best);
				best = current;
				++current;
			}
		}
	}
	if(choices.size()==1) return true;
	else return false;
}

SingleFunction* OverloadedFunction::getBestFit(const PositionID id, const std::vector<const AbstractClass*>& args) const{
	if(isGeneric!=nullptr){
		for(auto& a: innerFuncs){
			bool perfect=true;
			for(unsigned i=0; i<a->proto->declarations.size(); i++){
				if(args[i]!=a->proto->declarations[i].declarationType){
					perfect=false;
					break;
				}
			}
			if(perfect) return a;
		}
		///..do generic thing...
		id.compilerError("Generic functions not implemented");
	}
	std::list<SingleFunction*> choices;
	for(auto& a: innerFuncs){
		if(a->proto->declarations.size()>=args.size()){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(args[i]->classType==CLASS_VOID){
					if(a->proto->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					}
				} else if(!args[i]->hasCast(a->proto->declarations[i].declarationType)){
					valid=false;
					break;
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->proto->declarations.size(); i++)
				if(a->proto->declarations[i].defaultValue==nullptr){
					valid=false;
					break;
				}
			if(!valid) continue;
			choices.push_back(a);
		}
	}
	if(choices.size()==0){
		String t = "No matching function for "+toClassArgString(myName, args)+" options are:\n";
		for(SingleFunction* const & a:innerFuncs){
			t+=a->proto->toString()+"\n";
		}
		id.error(t);
	}
	for(unsigned int i=0; i<args.size() && choices.size()>1; i++){
		std::list<SingleFunction*>::iterator best=choices.begin();
		std::list<SingleFunction*>::iterator current=choices.begin();
		++current;
		for(; current!=choices.end();){
			//less means better
			auto c=args[i]->compare((*best)->proto->declarations[i].declarationType, (*current)->proto->declarations[i].declarationType);
			if(c==0){
				++current;
			} else if(c<0){
				auto tmp=current;
				++current;
				choices.erase(tmp);
			} else{
				choices.erase(best);
				best = current;
				++current;
			}
		}
	}
	if(choices.size()==1) return choices.front();
	else{
		String t = "Ambiguous function for "+toClassArgString(myName, args)+" options are:\n";
		for(auto& b: choices){
			t+=b->proto->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
}

SingleFunction* OverloadedFunction::getBestFit(const PositionID id, const std::vector<const Evaluatable*>& args) const{
	if(isGeneric!=nullptr){
		for(auto& a: innerFuncs){
			bool perfect=true;
			for(unsigned i=0; i<a->proto->declarations.size(); i++){
				if(args[i]->getReturnType()!=a->proto->declarations[i].declarationType){
					perfect=false;
					break;
				}
			}
			if(perfect) return a;
		}
		///..do generic thing...
		id.compilerError("Generic functions not implemented");
	}
	std::list<SingleFunction*> choices;
	for(auto& a: innerFuncs){
		if(a->proto->declarations.size()>=args.size()){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				const AbstractClass* const at = args[i]->getReturnType();
				assert(at);
				if(at->classType==CLASS_VOID){
					if(a->proto->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					}
				} else if(!args[i]->hasCastValue(a->proto->declarations[i].declarationType)){
					valid=false;
					break;
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->proto->declarations.size(); i++)
				if(a->proto->declarations[i].defaultValue==nullptr){
					valid=false;
					break;
				}
			if(!valid) continue;
			choices.push_back(a);
		}
	}
	if(choices.size()==0){
		String t = "No matching function for "+toClassArgString(myName, args)+" options are:\n";
		for(SingleFunction* const & a:innerFuncs){
			t+=a->proto->toString()+"\n";
		}
		id.error(t);
	}
	for(unsigned int i=0; i<args.size() && choices.size()>1; i++){
		std::list<SingleFunction*>::iterator best=choices.begin();
		std::list<SingleFunction*>::iterator current=choices.begin();
		++current;
		for(; current!=choices.end();){
			//less means better
			auto c=args[i]->compareValue(
					(*best)->proto->declarations[i].declarationType,
					(*current)->proto->declarations[i].declarationType);
			if(c==0){
				++current;
			} else if(c<0){
				auto tmp=current;
				++current;
				choices.erase(tmp);
			} else{
				choices.erase(best);
				best = current;
				++current;
			}
		}
	}
	if(choices.size()==1) return choices.front();
	else{
		String t = "Ambiguous function for "+toClassArgString(myName, args)+" options are:\n";
		for(auto& b: choices){
			t+=b->proto->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
}
#endif /* ABSTRACTFUNCTIONP_HPP_ */
