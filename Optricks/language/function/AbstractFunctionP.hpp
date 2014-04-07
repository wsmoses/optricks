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


	const Data* CompiledFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args) const{
		assert(myFunc);
		assert(myFunc->getReturnType());
		Value* cal = getRData().builder.CreateCall(myFunc,validatePrototypeNow(proto,r,id,args));
		if(proto->returnType->classType==CLASS_VOID) return &VOID_DATA;
		else{
			return new ConstantData(cal,proto->returnType);
		}
	}
BuiltinInlineFunction::BuiltinInlineFunction(FunctionProto* fp, std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&)> tmp):
SingleFunction(fp,getF(fp)),inlined(tmp){
	BasicBlock *Parent = getRData().builder.GetInsertBlock();
	BasicBlock *BB = getRData().CreateBlockD("entry", myFunc);
	getRData().builder.SetInsertPoint(BB);

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
	const Data* ret = inlined(getRData(), PositionID(0,0,"#inliner"), args);
	if(! getRData().hadBreak()){
		if(proto->returnType->classType==CLASS_VOID)
			getRData().builder.CreateRetVoid();
		else{
			Value* V = ret->getValue(getRData(),PositionID(0,0,"#inliner"));
			getRData().builder.CreateRet(V);
		}
	}
	getRData().FinalizeFunctionD(myFunc);
	if(Parent) getRData().builder.SetInsertPoint( Parent );
}

inline llvm::Function* BuiltinInlineFunction::getF(FunctionProto* fp){
	auto tmp=fp->declarations.size();
	llvm::SmallVector<Type*,0> ar(tmp);
	for(unsigned i=0; i<tmp; i++){
		ar[i] = fp->declarations[i].declarationType->type;
		assert(ar[i]);
	}
	llvm::Type* const T = (fp->returnType == &charClass)?CHARTYPE:fp->returnType->type;
	assert(T);
	llvm::FunctionType* FT = FunctionType::get(T, ar, false);
	return getRData().CreateFunctionD(fp->name, FT, LOCAL_FUNC);
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
	std::vector<const Evaluatable*> arg2;
	const auto ts = (as<=ds)?as:ds;
	for(unsigned int i = 0; i<ts; i++){
		const AbstractClass* const t = proto->declarations[i].declarationType;
		const AbstractClass* const at = args[i]->getReturnType();
		if(at->classType==CLASS_VOID){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}

			arg2.push_back(deLazy(r,id,proto->declarations[i].defaultValue,t));
		}
		else
			arg2.push_back(deLazy(r,id,args[i],t));
		assert(arg2.back());
	}
	if(as>ds){
		if(!proto->varArg)
			id.error("Gave too many arguments to function "+proto->toString());
		else
			for(unsigned int i=ts; i<as; i++)
				arg2.push_back(args[i]->evaluate(r)->toValue(r,id));
		return arg2;
	} else{
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
}
const Evaluatable* SingleFunction::deLazy(RData& r, PositionID id, Data* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		if(t==val->getReturnType()) return val;
		else return new CastEval(val,t,id);
	} /*else if(t->classType==CLASS_REF){
		if(val->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = val->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return val;
	} */else {
		return val->castTo(r, t, id);
	}
}
const Evaluatable* SingleFunction::deLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		if(t==val->getReturnType()) return val;
		else return new CastEval(val,t,id);
	} /*else if(t->classType==CLASS_REF){
		const Data* tt = val->evaluate(r);
		if(tt->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = tt->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return tt;
	} */else {
		return val->evaluate(r)->castTo(r, t, id)->toValue(r, id);
	}
}
Value* SingleFunction::fixLazy(RData& r, PositionID id, const Data* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		BasicBlock *Parent = r.builder.GetInsertBlock();
		const LazyClass* const lc = (const LazyClass*)t;
		FunctionType *FT = (llvm::FunctionType*)(((llvm::PointerType*)lc->type)->getElementType());
		Function* F = Function::Create(FT,LOCAL_FUNC,"%lazy",r.lmod);
		BasicBlock *BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		r.builder.CreateRet(val->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
		r.lmod->dump();
		cerr << endl << flush;
		return F;
	} /*else if(t->classType==CLASS_REF){
		if(val->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = val->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return ((LocationData*)val)->value->getPointer(r, id);
	} */else {
		return val->castToV(r, t, id);
	}
}
Value* SingleFunction::fixLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		BasicBlock *Parent = r.builder.GetInsertBlock();
		const LazyClass* lc = (const LazyClass*)t;
		FunctionType *FT = (llvm::FunctionType*)(((llvm::PointerType*)lc->type)->getElementType());
		Function* F = Function::Create(FT,LOCAL_FUNC,"%lazy",r.lmod);
		BasicBlock *BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		r.builder.CreateRet(val->evaluate(r)->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
		return F;
	} /*else if(t->classType==CLASS_REF){
		const Data* tt = val->evaluate(r);
		if(tt->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = tt->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return ((LocationData*)tt)->value->getPointer(r, id);
	} */else {
		return val->evaluate(r)->castToV(r, t, id);
	}
}
llvm::SmallVector<Value*,0> SingleFunction::validatePrototypeNow(FunctionProto* proto, RData& r,PositionID id,const std::vector<const Evaluatable*>& args){
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	const auto ts = (as<=ds)?as:ds;
	llvm::SmallVector<Value*,0> temp((ds>=as)?ds:as);
	for(unsigned int i = 0; i<ts; i++){
		const AbstractClass* const t = proto->declarations[i].declarationType;
		const AbstractClass* const at = args[i]->getReturnType();
		if(at->classType==CLASS_VOID){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1)+" for function "+proto->toString());
				exit(1);
			}
			temp[i] = fixLazy(r, id, proto->declarations[i].defaultValue, t);
		}
		else{
			temp[i] = fixLazy(r, id, args[i], t);
		}
		assert(temp[i]);
		assert(temp[i]->getType());
	}

	if(as>ds){
		if(!proto->varArg)
			id.error("Gave too many arguments to function "+proto->toString());
		else
			for(unsigned int i=ts; i<as; i++)
				temp[i] = args[i]->evaluate(r)->getValue(r,id);
		return temp;
	} else{
		for(unsigned int i = as; i<ds; i++){
			if(proto->declarations[i].defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1)+" for function "+proto->toString());
				exit(1);
			}
			const AbstractClass* const t = proto->declarations[i].declarationType;
			temp[i] = fixLazy(r, id, proto->declarations[i].defaultValue, t);
			assert(temp[i]);
			assert(temp[i]->getType());
		}
		return temp;
	}
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
			temp = fixLazy(r, id, args[i], t);
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
		V = r.builder.CreateInsertValue(V, fixLazy(r, id, proto->declarations[i].defaultValue, t), i);
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
	llvm::SmallVector<Type*,0> args(fp->declarations.size());
	for(unsigned i = 0; i<args.size(); i++) args[i] = fp->declarations[i].declarationType->type;
	auto gt = fp->getGeneratorType()->type;
	FunctionType *FT = FunctionType::get(gt, args, false);
	Function* F = r.CreateFunctionD(fp->name,FT,LOCAL_FUNC);
	BasicBlock *Parent = r.builder.GetInsertBlock();
	BasicBlock *BB = BasicBlock::Create(r.lmod->getContext(), "entry", F);
	r.builder.SetInsertPoint(BB);
	assert(dyn_cast<VectorType>(gt)!=nullptr);
	Value *V = UndefValue::get(gt);
	auto const tmp=fp->declarations.size();
	unsigned Idx = 0;
	for (Function::arg_iterator AI = F->arg_begin(); Idx != tmp;
			++AI, ++Idx)
		V = r.builder.CreateInsertValue(V, (Value*)AI, llvm::SmallVector<unsigned int,1>({Idx}));
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
		return getBestFit(id, fc->argumentTypes)->getSingleFunc();
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
		if(a->getSingleProto()->declarations.size()>=args.size()){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(args[i]->classType==CLASS_VOID){
					if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					}
				} else if(!args[i]->hasCast(a->getSingleProto()->declarations[i].declarationType)){
					valid=false;
					break;
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->getSingleProto()->declarations.size(); i++)
				if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
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
			auto c=args[i]->compare((*best)->getSingleProto()->declarations[i].declarationType, (*current)->getSingleProto()->declarations[i].declarationType);
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
			for(unsigned i=0; i<a->getSingleProto()->declarations.size(); i++){
				if(args[i]!=a->getSingleProto()->declarations[i].declarationType){
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
		if(a->getSingleProto()->declarations.size()>=args.size() || a->getSingleProto()->varArg){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(i>=a->getSingleProto()->declarations.size()) continue;
				const AbstractClass* const dt = a->getSingleProto()->declarations[i].declarationType;
				if(args[i]->classType==CLASS_VOID){
					if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					} else if(!a->getSingleProto()->declarations[i].defaultValue->hasCastValue(dt)){
						if(dt->classType!=CLASS_LAZY
							|| !a->getSingleProto()->declarations[i].defaultValue->hasCastValue(((LazyClass*) dt)->innerType)){
							valid = false;
							break;
						}
					}
				} else if(!args[i]->hasCast(dt)){
					if(dt->classType!=CLASS_LAZY
						|| !args[i]->hasCast(((LazyClass*) dt)->innerType)){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->getSingleProto()->declarations.size(); i++) {
				const AbstractClass* const dt = a->getSingleProto()->declarations[i].declarationType;
				if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
					valid=false;
					break;
				} else if(!a->getSingleProto()->declarations[i].defaultValue->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| !a->getSingleProto()->declarations[i].defaultValue->hasCastValue(((LazyClass*) dt)->innerType)){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			choices.push_back(a);
		}
	}
	if(choices.size()==0){
		String t = "No matching function for "+toClassArgString(myName, args)+" options are:\n";
		for(SingleFunction* const & a:innerFuncs){
			t+=a->getSingleProto()->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
	for(unsigned int i=0; i<args.size() && choices.size()>1; i++){
		std::list<SingleFunction*>::iterator best=choices.begin();
		std::list<SingleFunction*>::iterator current=choices.begin();
		++current;
		for(; current!=best;){
			//less means better
			//cerr << "C1 " << i << endl << flush;
			if(i>=(*best)->getSingleProto()->declarations.size() ||
					i>=(*current)->getSingleProto()->declarations.size()){
				++current;
				continue;
			}
			auto c1 = (*best)->getSingleProto()->declarations[i].declarationType;
			if(c1->classType==CLASS_LAZY) c1 = ((LazyClass*)c1)->innerType;
			auto c2 = (*current)->getSingleProto()->declarations[i].declarationType;
			if(c2->classType==CLASS_LAZY) c2 = ((LazyClass*)c2)->innerType;
			auto c=args[i]->compare(c1,c2);
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
			if(current == choices.end()) current = choices.begin();
		}
	}
	//cerr << "C1-B " << endl << flush;
	if(choices.size()==1) return choices.front();
	else{
		String t = "Ambiguous function for "+toClassArgString(myName, args)+" options are:\n";
		for(auto& b: choices){
			t+=b->getSingleProto()->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
}

SingleFunction* OverloadedFunction::getBestFit(const PositionID id, const std::vector<const Evaluatable*>& args) const{
	if(isGeneric!=nullptr){
		for(auto& a: innerFuncs){
			bool perfect=true;
			for(unsigned i=0; i<a->getSingleProto()->declarations.size(); i++){
				if(args[i]->getReturnType()!=a->getSingleProto()->declarations[i].declarationType){
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
		if(a->getSingleProto()->declarations.size()>=args.size() || a->getSingleProto()->varArg){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				const AbstractClass* const at = args[i]->getReturnType();
				assert(at);
				if(i>=a->getSingleProto()->declarations.size()) continue;
				const AbstractClass* const dt = a->getSingleProto()->declarations[i].declarationType;
				if(at->classType==CLASS_VOID){
					if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
						valid=false;
						break;
					} else if(!a->getSingleProto()->declarations[i].defaultValue->hasCastValue(dt)){
						if(dt->classType!=CLASS_LAZY
							|| !a->getSingleProto()->declarations[i].defaultValue->hasCastValue(((LazyClass*) dt)->innerType)){
							valid = false;
							break;
						}
					}
				} else if(!args[i]->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| !args[i]->hasCastValue(((LazyClass*) dt)->innerType)){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->getSingleProto()->declarations.size(); i++){
				const AbstractClass* const dt = a->getSingleProto()->declarations[i].declarationType;
				if(a->getSingleProto()->declarations[i].defaultValue==nullptr){
					valid=false;
					break;
				} else if(!a->getSingleProto()->declarations[i].defaultValue->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| !a->getSingleProto()->declarations[i].defaultValue->hasCastValue(((LazyClass*) dt)->innerType)){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			choices.push_back(a);
		}
	}
	if(choices.size()==0){
		String t = "No matching function for "+toClassArgString(myName, args)+" options are:\n";
		for(SingleFunction* const & a:innerFuncs){
			t+=a->getSingleProto()->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
	for(unsigned int i=0; i<args.size() && choices.size()>1; i++){
		std::list<SingleFunction*>::iterator best=choices.begin();
		std::list<SingleFunction*>::iterator current=choices.begin();
		++current;
		for(; current!=best;){
			//less means better
			if(i>=(*best)->getSingleProto()->declarations.size() ||
					i>=(*current)->getSingleProto()->declarations.size()){
				++current;
				continue;
			}
			auto c1 = (*best)->getSingleProto()->declarations[i].declarationType;
			if(c1->classType==CLASS_LAZY) c1 = ((LazyClass*)c1)->innerType;
			auto c2 = (*current)->getSingleProto()->declarations[i].declarationType;
			if(c2->classType==CLASS_LAZY) c2 = ((LazyClass*)c2)->innerType;
			auto c=args[i]->compareValue(c1,c2);
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
			if(current == choices.end()) current = choices.begin();
		}
	}
	if(choices.size()==1) return choices.front();
	else{
		String t = "Ambiguous function for "+toClassArgString(myName, args)+" options are:\n";
		for(auto& b: choices){
			t+=b->getSingleProto()->toString()+"\n";
		}
		id.error(t);
		exit(1);
	}
}
#endif /* ABSTRACTFUNCTIONP_HPP_ */
