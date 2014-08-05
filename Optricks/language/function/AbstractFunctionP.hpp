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
#include "../data/LazyWrapperData.hpp"

const Data* ExternalFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const{
	llvm::Value* cal = getRData().builder.CreateCall(getSingleFunc(),validatePrototypeNow(proto,r,id,args, instance));
	if(proto->returnType->classType==CLASS_VOID) return &VOID_DATA;
	else if(proto->returnType->classType==CLASS_REF)
		return new ReferenceData(new LocationData(new StandardLocation(cal), ((ReferenceClass*)proto->returnType)->innerType));
	else{
		return new ConstantData(cal,proto->returnType);
	}
}

llvm::Constant* ExternalFunction::getSingleFunc() const{
	if(myFunc) return myFunc;
	auto tmp=proto->declarations.size();
	llvm::SmallVector<llvm::Type*,0> ar(tmp);
	for(unsigned i=0; i<tmp; i++){
		ar[i] = proto->declarations[i].declarationType->type;
		assert(ar[i]);
	}
	auto FT = llvm::FunctionType::get(proto->returnType->type, ar, proto->varArg);
	return myFunc = getRData().getExtern(proto->name, FT, lib);
}

template<decltype(llvm::Intrinsic::sqrt) A>
const Data* IntrinsicFunction<A>::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const{
	llvm::Value* cal = getRData().builder.CreateCall(getSingleFunc(),validatePrototypeNow(proto,r,id,args, instance));
	if(proto->returnType->classType==CLASS_VOID) return &VOID_DATA;
	else if(proto->returnType->classType==CLASS_REF)
		return new ReferenceData(new LocationData(new StandardLocation(cal), ((ReferenceClass*)proto->returnType)->innerType));
	else{
		return new ConstantData(cal,proto->returnType);
	}
}

template<decltype(llvm::Intrinsic::sqrt) A>
llvm::Function* IntrinsicFunction<A>::getSingleFunc() const{
	if(myFunc) return (llvm::Function*)myFunc;
	auto tmp=proto->declarations.size();
	llvm::SmallVector<llvm::Type*,0> ar(tmp);
	for(unsigned i=0; i<tmp; i++){
		ar[i] = proto->declarations[i].declarationType->type;
		assert(ar[i]);
	}
	llvm::Function* F = llvm::Intrinsic::getDeclaration(getRData().lmod, A,ar);
	myFunc = F;
	return F;
}

	CompiledFunction::CompiledFunction(FunctionProto* const fp, llvm::Constant* const f):SingleFunction(fp,f){
		assert(llvm::dyn_cast<llvm::PointerType>(f->getType()));
		assert(llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*)f->getType())->getElementType()));
		assert(fp->returnType->classType==CLASS_VOID || llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*)f->getType())->getElementType())->getReturnType()==fp->returnType->type);
		assert(llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*)f->getType())->getElementType())->getNumParams()==fp->declarations.size());
		assert(llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*)f->getType())->getElementType())->isVarArg()==fp->varArg);
		for(unsigned i=0; i<fp->declarations.size(); i++){
			assert(llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*)f->getType())->getElementType())->getParamType(i)==fp->declarations[i].declarationType->type);
		}
	}
	const Data* BuiltinCompiledFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const{
			llvm::Value* cal = r.builder.CreateCall(getSingleFunc(),validatePrototypeNow(proto,r,id,args, instance));
			assert(cal);
			assert(cal->getType());
			assert(cal->getType()==proto->returnType->type);
			if(proto->returnType->classType==CLASS_VOID) return &VOID_DATA;
			else{
				return new ConstantData(cal,proto->returnType);
			}
		}
	const Data* CompiledFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const{
		assert(myFunc);
		assert(llvm::dyn_cast<llvm::PointerType>(myFunc->getType()));
		assert(llvm::dyn_cast<llvm::FunctionType>(((llvm::PointerType*) myFunc->getType())->getElementType()));
		llvm::Value* cal = r.builder.CreateCall(myFunc,validatePrototypeNow(proto,r,id,args, instance));
		if(proto->returnType->classType==CLASS_VOID) return &VOID_DATA;
		else if(proto->returnType->classType==CLASS_REF)
			return new ReferenceData(new LocationData(new StandardLocation(cal), ((ReferenceClass*)proto->returnType)->innerType));
		else{
			return new ConstantData(cal,proto->returnType);
		}
	}
	/*CompiledFunction* CompiledFunction::compile(FunctionProto* proto, std::function<const Data*(RData&,PositionID,const std::vector<const Evaluatable*>&,const Data*)> inlined){
		llvm::Function* myFunc = BuiltinInlineFunction::getF(proto);
		llvm::BasicBlock* Parent = getRData().builder.GetInsertBlock();
		llvm::BasicBlock* BB = getRData().CreateBlockD("entry", ((llvm::Function*)myFunc));
		getRData().builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		std::vector<const Evaluatable*> args;
		for (llvm::Function::arg_iterator AI = ((llvm::Function*)myFunc)->arg_begin(); Idx != ((llvm::Function*)myFunc)->arg_size();
				++AI, ++Idx) {
			((llvm::Value*)AI)->setName(llvm::Twine(proto->declarations[Idx].declarationVariable));
			if(proto->declarations[Idx].declarationType->classType==CLASS_REF)
				args.push_back(new LocationData(new StandardLocation(AI),proto->declarations[Idx].declarationType));
			else
				args.push_back(new ConstantData(AI,proto->declarations[Idx].declarationType));
		}
		//ASSUMES NO INLINE LOCAL METHODS
		const Data* ret = inlined(getRData(), PositionID(0,0,"#inliner"), args,nullptr);
		if(! getRData().hadBreak()){
			if(proto->returnType->classType==CLASS_VOID)
				getRData().builder.CreateRetVoid();
			else{
				llvm::Value* V = ret->getValue(getRData(),PositionID(0,0,"#inliner"));
				getRData().builder.CreateRet(V);
			}
		}
		getRData().FinalizeFunctionD((llvm::Function*)myFunc);
		if(Parent) getRData().builder.SetInsertPoint( Parent );
		return new CompiledFunction(proto, myFunc);
	}*/

	llvm::Function* BuiltinInlineFunction::getSingleFunc() const{
		if(myFunc!=nullptr) return (llvm::Function*)myFunc;
		myFunc = getF(proto);
		llvm::BasicBlock* Parent = getRData().builder.GetInsertBlock();
		llvm::BasicBlock* BB = getRData().CreateBlockD("entry", ((llvm::Function*)myFunc));
		getRData().builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		std::vector<const Evaluatable*> args;
		for (llvm::Function::arg_iterator AI = ((llvm::Function*)myFunc)->arg_begin(); Idx != ((llvm::Function*)myFunc)->arg_size();
				++AI, ++Idx) {
			((llvm::Value*)AI)->setName(llvm::Twine(proto->declarations[Idx].declarationVariable));
			if(proto->declarations[Idx].declarationType->classType==CLASS_REF)
				args.push_back(new LocationData(new StandardLocation(AI),proto->declarations[Idx].declarationType));
			else
				args.push_back(new ConstantData(AI,proto->declarations[Idx].declarationType));
		}
		//ASSUMES NO INLINE LOCAL METHODS
		const Data* ret = inlined(getRData(), PositionID(0,0,"#inliner"), args,nullptr);
		if(! getRData().hadBreak()){
			if(proto->returnType->classType==CLASS_VOID)
				getRData().builder.CreateRetVoid();
			else{
				llvm::Value* V = ret->getValue(getRData(),PositionID(0,0,"#inliner"));
				getRData().builder.CreateRet(V);
			}
		}
		getRData().FinalizeFunctionD((llvm::Function*)myFunc);
		if(Parent) getRData().builder.SetInsertPoint( Parent );
		return (llvm::Function*)myFunc;
	}
	llvm::Function* BuiltinCompiledFunction::getSingleFunc() const{
			if(myFunc!=nullptr) return (llvm::Function*)myFunc;
			myFunc = BuiltinInlineFunction::getF(proto);
			llvm::BasicBlock* Parent = getRData().builder.GetInsertBlock();
			llvm::BasicBlock* BB = getRData().CreateBlockD("entry", ((llvm::Function*)myFunc));
			getRData().builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			std::vector<const Evaluatable*> args;
			for (llvm::Function::arg_iterator AI = ((llvm::Function*)myFunc)->arg_begin(); Idx != ((llvm::Function*)myFunc)->arg_size();
					++AI, ++Idx) {
				((llvm::Value*)AI)->setName(llvm::Twine(proto->declarations[Idx].declarationVariable));
				if(proto->declarations[Idx].declarationType->classType==CLASS_REF)
					args.push_back(new LocationData(new StandardLocation(AI),proto->declarations[Idx].declarationType));
				else
					args.push_back(new ConstantData(AI,proto->declarations[Idx].declarationType));
			}
			//ASSUMES NO INLINE LOCAL METHODS
			const Data* ret = inlined(getRData(), PositionID(0,0,"#inliner"), args,nullptr);
			if(! getRData().hadBreak()){
				if(proto->returnType->classType==CLASS_VOID)
					getRData().builder.CreateRetVoid();
				else{
					llvm::Value* V = ret->getValue(getRData(),PositionID(0,0,"#inliner"));
					getRData().builder.CreateRet(V);
				}
			}
			getRData().FinalizeFunctionD((llvm::Function*)myFunc);
			if(Parent) getRData().builder.SetInsertPoint( Parent );
			return (llvm::Function*)myFunc;
		}

inline llvm::Function* BuiltinInlineFunction::getF(FunctionProto* fp){
	auto tmp=fp->declarations.size();
	llvm::SmallVector<llvm::Type*,0> ar(tmp);
	for(unsigned i=0; i<tmp; i++){
		ar[i] = fp->declarations[i].declarationType->type;
		assert(ar[i]);
	}
	llvm::Type* const T = (fp->returnType == &charClass)?CHARTYPE:fp->returnType->type;
	assert(fp->returnType);
	assert(T || PositionID(0,0,"#getF").warning(fp->returnType->getName()+"  has no type"));;
	llvm::FunctionType* FT = llvm::FunctionType::get(T, ar, false);
	return getRData().CreateFunctionD(fp->name, FT, LOCAL_FUNC);
}
String toClassArgString(String funcName, const std::vector<const AbstractClass*>& args){
	String s=funcName+"(";
	bool first=true;
	for(const auto& a:args){
		assert(a);
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
		if(a)
		s+=a->getReturnType()->getName();
		else s+="<none>";
	}
	return s+")";
}


std::vector<const Evaluatable*> SingleFunction::validatePrototypeInline(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data*& instance) const {
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	std::vector<const Evaluatable*> arg2;
	const auto ts = (as<=ds)?as:ds;
	if(instance){
		auto TMP = proto->declarations[0].declarationType;
		if(TMP->classType==CLASS_REF) TMP = ((ReferenceClass*)TMP)->innerType;
		if(instance->getReturnType() != TMP){
			instance = instance->castTo(r, TMP, id);
		}
	}
	for(unsigned int i = 0; i<ts; i++){
		auto myDec = proto->declarations[i+(instance?1:0)];
		const AbstractClass* const t = myDec.declarationType;
		if(args[i]==nullptr){
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}

			arg2.push_back(deLazyInline(r,id,myDec.defaultValue,t));
		}
		else
			arg2.push_back(deLazyInline(r,id,args[i],t));
		assert(arg2.back());
	}
	if(as+(instance?1:0)>ds){
		if(!proto->varArg)
			id.error("Gave too many arguments to function "+proto->toString());
		else
			for(unsigned int i=ts; i<as; i++)
				arg2.push_back(args[i]->evaluate(r)->toValue(r,id));
		return arg2;
	} else{
		for(unsigned int i = as; i+(instance?1:0)<ds; i++){
			auto myDec = proto->declarations[i+(instance?1:0)];
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}
			const AbstractClass* const t = myDec.declarationType;
			arg2.push_back(deLazyInline(r,id,myDec.defaultValue,t));
		}
		return arg2;
	}
}
const Evaluatable* SingleFunction::deLazyInline(RData& r, PositionID id, Data* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		const AbstractClass* VRT = val->getReturnType();
		if(t==VRT || t->classType==CLASS_VOID) return val;
		if(VRT->classType!=CLASS_LAZY){
			if(((LazyClass*)t)->innerType==VRT || ((LazyClass*)t)->innerType->classType==CLASS_VOID) return new LazyWrapperData(val);
			else return new LazyWrapperData(new CastEval(val, ((LazyClass*)t)->innerType, id));
		}
		else return new CastEval(val,t,id);
	} /*else if(t->classType==CLASS_REF){
		if(val->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = val->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return val;
	} */else {
		if(t->classType==CLASS_VOID) return val;
		return val->castTo(r, t, id);
	}
}
const Evaluatable* SingleFunction::deLazyInline(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) {
	if(t->classType==CLASS_LAZY){
		const AbstractClass* VRT = val->getReturnType();
		if(t==VRT || t->classType==CLASS_VOID) return val;
		if(VRT->classType!=CLASS_LAZY){
			if(((LazyClass*)t)->innerType==VRT || ((LazyClass*)t)->innerType->classType==CLASS_VOID) return new LazyWrapperData(val);
			else return new LazyWrapperData(new CastEval(val, ((LazyClass*)t)->innerType, id));
		}
		else return new CastEval(val,t,id);
	} /*else if(t->classType==CLASS_REF){
		const Data* tt = val->evaluate(r);
		if(tt->type!=R_LOC) id.error("Cannot use non-variable as argument for function requiring reference");
		const ReferenceClass* rc = (const ReferenceClass*)t;
		auto tmp = tt->getReturnType();
		if(tmp!=rc->innerType) id.error("Cannot use "+tmp->getName()+"& in place of "+rc->getName());
		return tt;
	} */else {
		if(t->classType==CLASS_VOID) return val->evaluate(r)->toValue(r, id);
		return val->evaluate(r)->castTo(r, t, id)->toValue(r, id);
	}
}
llvm::Value* SingleFunction::fixLazy(RData& r, PositionID id, const Data* val, const AbstractClass* const t) {
	assert(val);
	if(t->classType==CLASS_LAZY){
		llvm::BasicBlock* Parent = r.builder.GetInsertBlock();
		const LazyClass* const lc = (const LazyClass*)t;
		llvm::FunctionType* FT = (llvm::FunctionType*)(((llvm::PointerType*)lc->type)->getElementType());
		llvm::Function* F = llvm::Function::Create(FT,LOCAL_FUNC,"%lazy",r.lmod);
		llvm::BasicBlock *BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		if(lc->innerType->classType==CLASS_VOID)
				r.builder.CreateRetVoid();
		else
			r.builder.CreateRet(val->castToV(r, lc->innerType, id));

		r.builder.CreateRet(val->castToV(r, lc->innerType, id));
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
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
llvm::Value* SingleFunction::fixLazy(RData& r, PositionID id, const Evaluatable* val, const AbstractClass* const t) {
	assert(val);
	if(t->classType==CLASS_LAZY){
		llvm::BasicBlock* Parent = r.builder.GetInsertBlock();
		const LazyClass* lc = (const LazyClass*)t;
		llvm::FunctionType* FT = (llvm::FunctionType*)(((llvm::PointerType*)lc->type)->getElementType());
		llvm::Function* F = llvm::Function::Create(FT,LOCAL_FUNC,"%lazy",r.lmod);
		llvm::BasicBlock* BB = r.CreateBlockD("entry", F);
		r.builder.SetInsertPoint(BB);
		const Data* D = val->evaluate(r);
		if(lc->innerType->classType==CLASS_VOID)
			r.builder.CreateRetVoid();
		else
			r.builder.CreateRet(D->castToV(r, lc->innerType, id));
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
llvm::SmallVector<llvm::Value*,0> SingleFunction::validatePrototypeNow(FunctionProto* proto, RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance){
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	const auto ts = (as<=ds)?as:ds;
	llvm::SmallVector<llvm::Value*,0> temp((ds>=as)?ds:as);
	if(instance){
		llvm::Value* T;
		if(proto->declarations[0].declarationType->classType==CLASS_REF){
			if(instance->type==R_CONST)
				T = ((const ConstantData*)instance)->toLocation(r, "this")->value->getPointer(r, id);
			else if(instance->type==R_LOC){
				T = ((LocationData*)instance)->value->getPointer(r, id);
			} else if(instance->type==R_DEC){
				T = ((DeclarationData*)instance)->value->fastEvaluate(r)->getPointer(r, id);
			} else {
				id.error("Cannot use constant "+str(instance->type)+" in place of location");
				exit(1);
			}
		} else {
			T = instance->getValue(r, id);
		}
		if(T->getType()!= proto->declarations[0].declarationType->type){
			assert(proto->declarations[0].declarationType->type->isPointerTy());
			T = r.pointerCast(T, (llvm::PointerType*) proto->declarations[0].declarationType->type);
		}
		assert(T != NULL);
		temp[0] = T;
	}
	for(unsigned int i = 0; i<ts; i++){
		auto myDec = proto->declarations[i+(instance?1:0)];
		const AbstractClass* const t = myDec.declarationType;
		llvm::Value* T;
		if(args[i]==nullptr){
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1)+" for function "+proto->toString());
				exit(1);
			}
			T = fixLazy(r, id, myDec.defaultValue, t);
		}
		else{
			T = fixLazy(r, id, args[i], t);
		}
		assert(T);
		assert(T->getType());
		temp[i+(instance?1:0)] = T;
	}

	if(as+(instance?1:0)>ds){
		if(!proto->varArg)
			id.error("Gave too many arguments to function "+proto->toString());
		else
			for(unsigned int i=ts; i<as; i++)
				temp[i+(instance?1:0)] = args[i]->evaluate(r)->getValue(r,id);
		return temp;
	} else{
		for(unsigned int i = as; i+(instance?1:0)<ds; i++){
			auto myDec = proto->declarations[i+(instance?1:0)];
			const AbstractClass* const t = myDec.declarationType;
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1)+" for function "+proto->toString());
				exit(1);
			}
			llvm::Value* V = fixLazy(r, id, myDec.defaultValue, t);
			assert(V);
			assert(V->getType());
			temp[i+(instance?1:0)] = V;
		}
		return temp;
	}
}
llvm::Value* SingleFunction::validatePrototypeStruct(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance, llvm::Value* V) const{
	const auto as = args.size();
	const auto ds = proto->declarations.size();
	if(as+(instance?1:0)>ds) id.error("Gave too many arguments to function "+proto->toString());
	if(ds==0){
		return V;
	}
	if(instance){
		llvm::Value* T;
		if(proto->declarations[0].declarationType->classType==CLASS_REF){
			if(instance->type==R_CONST)
				T = ((const ConstantData*)instance)->toLocation(r, "this")->value->getPointer(r, id);
			else if(instance->type==R_LOC){
				T = ((LocationData*)instance)->value->getPointer(r, id);
			} else if(instance->type==R_DEC){
				T = ((DeclarationData*)instance)->value->fastEvaluate(r)->getPointer(r, id);
			} else {
				id.error("Cannot use constant "+str(instance->type)+" in place of location");
				exit(1);
			}
		} else {
			T = instance->getValue(r, id);
		}
		if(T->getType()!= proto->declarations[0].declarationType->type){
			assert(proto->declarations[0].declarationType->type->isPointerTy());
			T = r.builder.CreatePointerCast(T, (llvm::PointerType*) proto->declarations[0].declarationType->type);
		}
		assert(T != NULL);
		if(ds==1) return T;
		else {
			assert(V);
			assert(V->getType());
			assert(llvm::dyn_cast<llvm::StructType>(V->getType()));
			assert(llvm::dyn_cast<llvm::StructType>(V->getType())->getNumElements()==ds);
		}
		assert(0<llvm::dyn_cast<llvm::StructType>(V->getType())->getNumElements());
		V = r.builder.CreateInsertValue(V, T, 0);
		assert(V);
	} else if(ds==1){
		auto myDec = proto->declarations[0];
		const AbstractClass* const t = myDec.declarationType;
		llvm::Value* temp;
		if(args.size()==0 || args[0]==nullptr){
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(1));
				exit(1);
			}
			temp = fixLazy(r, id, myDec.defaultValue, t);
		}
		else{
			temp = fixLazy(r, id, args[0], t);
		}
		return temp;
	} else{
		assert(V);
		assert(V->getType());
		assert(llvm::dyn_cast<llvm::StructType>(V->getType()));
		assert(llvm::dyn_cast<llvm::StructType>(V->getType())->getNumElements()==ds);
	}
	for(unsigned int i = 0; i<as; i++){
		auto myDec = proto->declarations[i+(instance?1:0)];
		const AbstractClass* const t = myDec.declarationType;
		llvm::Value* temp;
		if(args[i]==nullptr){
			if(myDec.defaultValue==nullptr){
				id.error("No default argument available for argument "+str(i+1));
				exit(1);
			}
			temp = fixLazy(r, id, myDec.defaultValue, t);
		}
		else{
			temp = fixLazy(r, id, args[i], t);
		}
		assert(i+(instance?1:0)<llvm::dyn_cast<llvm::StructType>(V->getType())->getNumElements());
		V = r.builder.CreateInsertValue(V, temp, i+(instance?1:0));
		assert(V);
	}
	for(unsigned int i = as; i+(instance?1:0)<ds; i++){
		auto myDec = proto->declarations[i+(instance?1:0)];
		const AbstractClass* const t = myDec.declarationType;
		if(myDec.defaultValue==nullptr){
			id.error("Error: No default argument available for argument "+str(i+1));
			exit(1);
		}
		assert(i<llvm::dyn_cast<llvm::StructType>(V->getType())->getNumElements());
		V = r.builder.CreateInsertValue(V, fixLazy(r, id, myDec.defaultValue, t), i);
		assert(V);
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
	return new ConstantData(castToV(r, right, id), right);
}

llvm::Value* SingleFunction::castToV(RData& r, const AbstractClass* const right, PositionID id) const{
	getSingleFunc();
	switch(right->classType){
	case CLASS_FUNC:{
		auto fc= proto->getFunctionClass();
		if(fc->noopCast(right)){
			assert(right->type->isPointerTy());
			if(right->type==myFunc->getType()) return myFunc;
			else return r.pointerCast(myFunc, (llvm::PointerType*) right->type);
		}
		else {
			id.error("Single Function automatic generation of types has not been implemented... "+fc->getName()+" to "+right->getName());
			exit(1);
		}
	}
	case CLASS_CPOINTER:
		return r.pointerCast(myFunc,C_POINTERTYPE);
	default:
		id.error("Cannot cast function to non-function class "+right->getName());
		exit(1);
	}
}

llvm::Function* const createGeneratorFunction(FunctionProto* const fp, RData& r, PositionID id){
	llvm::SmallVector<llvm::Type*,0> args(fp->declarations.size());
	for(unsigned i = 0; i<args.size(); i++) args[i] = fp->declarations[i].declarationType->type;
	auto gt = fp->returnType->type;
	llvm::FunctionType *FT = llvm::FunctionType::get(gt, args, false);
	llvm::Function* F = r.CreateFunctionD(fp->name,FT,LOCAL_FUNC);
	llvm::BasicBlock* Parent = r.builder.GetInsertBlock();
	llvm::BasicBlock* BB = llvm::BasicBlock::Create(r.lmod->getContext(), "entry", F);
	r.builder.SetInsertPoint(BB);
	if(args.size()==0)
		r.builder.CreateRetVoid();
	else if(args.size()==1)
		r.builder.CreateRet(F->arg_begin());
	else {
		assert(llvm::dyn_cast<llvm::StructType>(gt)!=nullptr);
		llvm::Value* V = getUndef(gt);
		auto const tmp=fp->declarations.size();
		unsigned Idx = 0;
		for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != tmp;
				++AI, ++Idx)
			V = r.builder.CreateInsertValue(V, (llvm::Value*)AI, llvm::SmallVector<unsigned int,1>({Idx}));
		r.builder.CreateRet(V);
	}
	if(Parent) r.builder.SetInsertPoint(Parent);
	return F;
}

const AbstractClass* GeneratorFunction::getMyClass(RData& r, PositionID id) const {
	return proto->returnType;
}

llvm::Function* GeneratorFunction::getSingleFunc() const{
		if(myFunc!=nullptr) return (llvm::Function*)myFunc;
		return (llvm::Function*)(myFunc = createGeneratorFunction(proto, getRData(), filePos));
}
const Data* GeneratorFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) const{
	auto gt=proto->returnType;
	llvm::Value* V = getUndef(gt->type);
	return new ConstantData(validatePrototypeStruct(r,id,args,instance,V),gt);
}


llvm::Value* OverloadedFunction::castToV(RData& r, const AbstractClass* const right, PositionID id) const {
	switch(right->classType){
	case CLASS_FUNC:{
		//todo .. have cast (no-op) wrapper on this
		FunctionClass* fc = (FunctionClass*)right;
		return getBestFit(id, fc->argumentTypes,false)->castToV(r, right, id);
	}
	case CLASS_CPOINTER:
		return r.pointerCast(getValue(r, id),C_POINTERTYPE);
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

const Data* OverloadedFunction::callFunction(RData& r,PositionID id,const std::vector<const Evaluatable*>& args, const Data* instance) const{
	auto gbf=getBestFit(id,args, instance!=nullptr);
	return gbf->callFunction(r,id,args, instance);
}
SingleFunction* OverloadedFunction::getBestFit(const PositionID id, const std::vector<const AbstractClass*>& args, bool isClassMethod) const{
	if(isGeneric!=nullptr){
		for(auto& a: innerFuncs){
			bool perfect=true;
			for(unsigned i=(isClassMethod?1:0); i<a->getSingleProto()->declarations.size(); i++){
				if(args[i-(isClassMethod?1:0)]!=a->getSingleProto()->declarations[i].declarationType){
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
		if(a->getSingleProto()->declarations.size()>=args.size()+(isClassMethod?1:0) || a->getSingleProto()->varArg){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(i+(isClassMethod?1:0)>=a->getSingleProto()->declarations.size()) continue;
				auto myDec = a->getSingleProto()->declarations[i+(isClassMethod?1:0)];
				const AbstractClass* const dt = myDec.declarationType;
				if(args[i]==nullptr){
					if(myDec.defaultValue==nullptr){
						valid=false;
						break;
					} else if(!myDec.defaultValue->hasCastValue(dt)){
						if(dt->classType!=CLASS_LAZY
							|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID && !myDec.defaultValue->hasCastValue(((LazyClass*) dt)->innerType))){
							valid = false;
							break;
						}
					}
				} else if(!args[i]->hasCast(dt)){
					if(dt->classType!=CLASS_LAZY
						|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID &&
								!args[i]->hasCast(((LazyClass*) dt)->innerType))){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i<a->getSingleProto()->declarations.size()-(isClassMethod?1:0); i++) {
				auto myDec = a->getSingleProto()->declarations[i+(isClassMethod?1:0)];
				const AbstractClass* const dt = myDec.declarationType;
				if(myDec.defaultValue==nullptr){
					valid=false;
					break;
				} else if(!myDec.defaultValue->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID &&
								!myDec.defaultValue->hasCastValue(((LazyClass*) dt)->innerType))){
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
			if(i+(isClassMethod?1:0)>=(*best)->getSingleProto()->declarations.size() ||
					i+(isClassMethod?1:0)>=(*current)->getSingleProto()->declarations.size()){
				++current;
				if(current == choices.end()) current = choices.begin();
				continue;
			}
			auto c1 = (*best)->getSingleProto()->declarations[i+(isClassMethod?1:0)].declarationType;
			auto c2 = (*current)->getSingleProto()->declarations[i+(isClassMethod?1:0)].declarationType;
			if(c1->classType==CLASS_LAZY) c1 = ((LazyClass*)c1)->innerType;
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

SingleFunction* OverloadedFunction::getBestFit(const PositionID id, const std::vector<const Evaluatable*>& args,bool isClassMethod) const{
	//force type construction / templated function generation
	//TODO why is below important again? -- to force println/etc to be registered
	if(this->myName=="print" || this->myName=="println")
		for(const auto& a: args) if(a){
			a->getReturnType();
		}
	if(isGeneric!=nullptr){
		for(auto& a: innerFuncs){
			bool perfect=true;
			for(unsigned i=(isClassMethod?1:0); i<a->getSingleProto()->declarations.size(); i++){
				if(args[i-(isClassMethod?1:0)]->getReturnType()!=a->getSingleProto()->declarations[i].declarationType){
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
		if(a->getSingleProto()->declarations.size()>=args.size()+(isClassMethod?1:0) || a->getSingleProto()->varArg){
			bool valid=true;
			for(unsigned int i=0; i<args.size(); i++){
				if(i+(isClassMethod?1:0)>=a->getSingleProto()->declarations.size()) continue;
				auto myDec = a->getSingleProto()->declarations[i+(isClassMethod?1:0)];
				const AbstractClass* const dt = myDec.declarationType;
				if(args[i]==nullptr){
					if(myDec.defaultValue==nullptr){
						valid=false;
						break;
					} else if(!myDec.defaultValue->hasCastValue(dt)){
						if(dt->classType!=CLASS_LAZY
							|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID &&
									!myDec.defaultValue->hasCastValue(((LazyClass*) dt)->innerType))){
							valid = false;
							break;
						}
					} else continue;
				} else if(!args[i]->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID &&
								!args[i]->hasCastValue(((LazyClass*) dt)->innerType))){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			for(unsigned int i=args.size(); i+(isClassMethod?1:0)<a->getSingleProto()->declarations.size(); i++){
				auto myDec = a->getSingleProto()->declarations[i+(isClassMethod?1:0)];
				const AbstractClass* const dt = myDec.declarationType;
				if(myDec.defaultValue==nullptr){
					valid=false;
					break;
				} else if(!myDec.defaultValue->hasCastValue(dt)){
					if(dt->classType!=CLASS_LAZY
						|| (((LazyClass*) dt)->innerType->classType!=CLASS_VOID &&
								!myDec.defaultValue->hasCastValue(((LazyClass*) dt)->innerType))){
						valid = false;
						break;
					}
				}
			}
			if(!valid) continue;
			else{
				choices.push_back(a);
			}
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
			if(i+(isClassMethod?1:0)>=(*best)->getSingleProto()->declarations.size() ||
					i+(isClassMethod?1:0)>=(*current)->getSingleProto()->declarations.size()){
				++current;
				if(current == choices.end()) current = choices.begin();
				continue;
			}
			auto c1 = (*best)->getSingleProto()->declarations[i+(isClassMethod?1:0)].declarationType;
			auto c2 = (*current)->getSingleProto()->declarations[i+(isClassMethod?1:0)].declarationType;
			assert(c1);
			assert(c2);
			if(c1->classType==CLASS_LAZY) c1 = ((LazyClass*)c1)->innerType;
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
