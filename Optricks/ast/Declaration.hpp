/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../language/statement/Statement.hpp"
#include "./E_VAR.hpp"
#include "../language/location/Location.hpp"
#include "../language/class/builtin/ReferenceClass.hpp"
#include "../language/class/builtin/FunctionClass.hpp"
#include "../language/class/builtin/LazyClass.hpp"
#include "../language/data/ReferenceData.hpp"
#include "../operators/Deconstructor.hpp"

#define DECLR_P_
class Declaration: public ErrorStatement{
private:
	Statement* classV;
	mutable const AbstractClass* returnType;
	mutable unsigned isReference;
public:
	const AbstractClass* getMyClass(RData& r, PositionID id)const{
				id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
			}
	const AbstractClass* getClass(RData& r, PositionID id)const{
		if(classV){
			return classV->getMyClass(r, id);
		}
		else{
			id.error("Cannot use auto declaration");
			exit(1);
		}
	}
	E_VAR variable;
	Statement* value;
	bool global;
	mutable const LocationData* finished;
	Declaration(PositionID id, Statement* v, const E_VAR& loc, bool glob, Statement* e) :
		ErrorStatement(id),
	classV(v),returnType(nullptr),isReference(2),variable(loc),value(e),global(glob),finished(nullptr){
	}
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
	}
	bool hasValue() const {
		return value!=NULL && value->getToken()!=T_VOID;
	}
	const Token getToken() const final override{
		return T_DECLARATION;
	}
	const AbstractClass* getReturnType() const final override{
		if(returnType) return returnType;
		if(!classV){
			if(value){
				returnType = value->getReturnType();
				if(returnType->classType==CLASS_VOID){
					filePos.error("Cannot have void declaration");
					exit(1);
				}
				if(returnType->classType==CLASS_REF){
					returnType = ((const ReferenceClass*)returnType)->innerType;
					isReference = 1;
				} else isReference = 0;
				return returnType;
			} else {
				filePos.error("Cannot have auto declaration without default value");
				//TODO implement searching for correct variable type
				exit(1);
			}
		}
		returnType = classV->getMyClass(getRData(), filePos);
		if(returnType->classType==CLASS_REF){
			returnType = ((ReferenceClass*)returnType)->innerType;
			isReference = 1;
		} else isReference = 0;
		assert(returnType);
		if(returnType->classType==CLASS_VOID){
			filePos.error("Cannot have void declaration");
			exit(1);
		}
		return returnType;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		assert(isClassMethod==false);
		getReturnType();
		if(returnType->classType==CLASS_FUNC){
			return ((FunctionClass*)returnType)->returnType;
		}  else if(returnType->classType==CLASS_LAZY){
			return ((LazyClass*)returnType)->innerType;
		} else if(returnType->classType==CLASS_CLASS){
			return returnType;
		}	else {
			id.error("Class '"+returnType->getName()+"' cannot be used as function");
			exit(1);
		}
	}
	void registerClasses() const override final{
		if(classV) classV->registerClasses();
		variable.registerClasses();
		if(value) value->registerClasses();
	}
	void registerFunctionPrototype(RData& r) const override final{
		if(classV) classV->registerFunctionPrototype(r);
		variable.registerFunctionPrototype(r);
		if(value) value->registerFunctionPrototype(r);
	};
	void buildFunction(RData& r) const override final{
		if(classV) classV->buildFunction(r);
		variable.buildFunction(r);
		if(value) value->buildFunction(r);
	};
	void reset() const override final{
		finished = nullptr;
	}
	const LocationData* fastEvaluate(RData& r){
		if(finished) return finished;
		getReturnType();
		assert(returnType);
		if(returnType->layout==LITERAL_LAYOUT){
			filePos.error("Cannot get value of literal, please wrap with finite class like int/string");
			exit(1);
		}
		assert(returnType->type);
		if(isReference == 1){
			filePos.error("Cannot find references early");
		}
		if(global){
			llvm::Constant* VAL;
			if(returnType->layout==POINTER_LAYOUT){
				assert(returnType->type);
				assert(llvm::dyn_cast<llvm::PointerType>(returnType->type));
				VAL = llvm::ConstantPointerNull::get((llvm::PointerType*)returnType->type);
			}
			else
				VAL = llvm::UndefValue::get(returnType->type);
			llvm::GlobalVariable* GV = new llvm::GlobalVariable(*r.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,VAL);
			((llvm::Value*)GV)->setName(llvm::Twine(variable.getFullName()));
			variable.getMetadata().setObject(finished=new LocationData(new StandardLocation(GV),returnType));
		}
		else{
			auto TheFunction = r.builder.GetInsertBlock()->getParent();
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
			auto al = TmpB.CreateAlloca(returnType->type, NULL,llvm::Twine(variable.pointer.name));
			variable.getMetadata().setObject(finished=new LocationData(getLazy(variable.pointer.name,r,al,nullptr,nullptr),returnType));
		}
		//todo check lazy for globals
		return finished;
	}
	const LocationData* evaluate(RData& r) const final override{
		if(finished){
			if(value){
				Location* aloc = finished->getMyLocation();
				assert(aloc);
				llvm::Value* nex = value->evaluate(r)->castToV(r, returnType, filePos);
				assert(nex);
				aloc->setValue(nex,r);
				incrementCount(r, filePos, finished);
			}
			return finished;
		}
		getReturnType();
		assert(returnType);
		const Data* D = (value==NULL || value->getToken()==T_VOID)?NULL:value->evaluate(r);
		if(isReference==1){
			if(!D){
				filePos.error("Cannot declare reference without value");
				exit(1);
			}
			auto RT = D->getReturnType();
			if(RT->classType!=CLASS_REF){
				filePos.error("Cannot create reference of non-reference type "+RT->getName());
				exit(1);
			}
			const ReferenceData* R = (const ReferenceData*)D;
			variable.getMetadata().setObject(R->value);
			filePos.warning("Garbage collection of references not implemented yet");
			return finished=R->value;
		}
		llvm::Value* tmp = (value==NULL || value->getToken()==T_VOID)?NULL:(D->castToV(r, returnType, filePos));
		assert(returnType->type);
		assert(!tmp || tmp->getType()==returnType->type);
		if(global){
			llvm::GlobalVariable* GV;
			if(auto cons = llvm::dyn_cast_or_null<llvm::Constant>(tmp))
				GV = new llvm::GlobalVariable(*r.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,cons);
			else{
				llvm::Constant* VAL;
				if(returnType->layout==POINTER_LAYOUT){
					assert(returnType->type);
					assert(llvm::dyn_cast<llvm::PointerType>(returnType->type));
					VAL = llvm::ConstantPointerNull::get((llvm::PointerType*)returnType->type);
				}
				else
					VAL = llvm::UndefValue::get(returnType->type);
				GV = new llvm::GlobalVariable(*r.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,VAL);
				if(tmp!=NULL) r.builder.CreateStore(tmp,GV);
			}
			((llvm::Value*)GV)->setName(llvm::Twine(variable.getFullName()));
			variable.getMetadata().setObject(finished=new LocationData(new StandardLocation(GV),returnType));
		}
		else{
			auto TheFunction = r.builder.GetInsertBlock()->getParent();
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),TheFunction->getEntryBlock().begin());
			auto al = TmpB.CreateAlloca(returnType->type, NULL,llvm::Twine(variable.pointer.name));
			variable.getMetadata().setObject(finished=new LocationData(getLazy(variable.pointer.name,r,al,(tmp)?r.builder.GetInsertBlock():nullptr,tmp),returnType));
		}
		//todo check lazy for globals

		incrementCount(r, filePos, finished);
		return finished;
	}
};

/*
void initFuncsMeta(RData& rd){
	//cout << "SIZE OF C_STR: " <<rd.lmod->getPointerSize() << endl << flush;
	//cout << "SIZE OF ANY: " << Module::PointerSize::AnyPointerSize << endl << flush;
	//cout << "SIZE OF x32: " << Module::PointerSize::Pointer32 << endl << flush;
	//cout << "SIZE OF x64: " << Module::PointerSize::Pointer64 << endl << flush;
	//cout << "SIZE OF PTR: " << sizeof(char*) << endl << flush;
	//TODO begin conversion of constructors to generators
	{
	}

	c_stringClass->addLocalFunction(PositionID(0,0,"<start.initFuncsMeta>"),"length").add(Data*::getFunction(o_strlen,new FunctionProto("length", intClass)),PositionID(0,0,"<start.initFuncsMeta>"));
	charClass->addCast(c_stringClass) = new ouopNative(
			[](Data* av, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m,id);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			APInt va = constantInt->getValue();
			char t = (char)(va.getSExtValue () );
			return new ConstantData(m.builder.CreateGlobalStringPtr(String(1,t),"tmpstr"),c_stringClass);
		}
		id.error("Cannot convert from non-const char to string");
		return VOID;
	},c_stringClass);
	/ *charClass->addCast(stringClass) = new ouopNative(
			[](Data* av, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m);
		if(ConstantInt* c = dyn_cast<ConstantInt>(a)){
			auto va = c->getValue();
			char t = (char)(va.getSExtValue () );
			Value* st = m.builder.CreateGlobalStringPtr(String(1,t),"tmpstr");
			Value* str = UndefValue::get(stringClass->getType(m));
			str= m.builder.CreateInsertValue(str,st,{0});
			str= m.builder.CreateInsertValue(str,getInt(1),{1});
			return new ConstantData(str,stringClass);
		}
		Value* str = UndefValue::get(stringClass->getType(m));
		Constant *StrConstant = ConstantDataArray::getString(getGlobalContext(), "a");
		Module *N = (m.builder.GetInsertBlock()->getParent()->getParent());
		Module &M = *N;
		GlobalVariable *GV = new GlobalVariable(M, StrConstant->getType(),
				false, GlobalValue::PrivateLinkage,StrConstant);
		GV->setName("idk");
		GV->setUnnamedAddr(true);
		Value *Args[] = {getInt32(0),getInt32(0)};

		Value* st = m.builder.CreateInBoundsGEP(GV, Args);

		m.builder.CreateStore(a,st);

		str= m.builder.CreateInsertValue(str,st,{0});
		str= m.builder.CreateInsertValue(str,getInt(1),{1});
		return new ConstantData(str,stringClass);
	},stringClass);* /
	//c_stringClass->addBinop("+",c_stringClass) = new obinopNative(
	//		[](Value* ay, Value* by, RData& m) -> Data*{
	//TODO string addition (need malloc)
	//},c_stringClass);

	intClass->addBinop("*",charClass) = new obinopNative(
			[](Data* av, Data* bv, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m,id);
		Value* b = bv.getValue(m,id);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(b)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(a)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				auto len = (va2.getSExtValue () );
				return new ConstantData(m.builder.CreateGlobalStringPtr(String(len,t),"tmpstr"),c_stringClass);
			}
		}
		PositionID(0,0,"!done").error("Could not find constant");
		return VOID;
	}
	,c_stringClass);
	charClass->addBinop("*",intClass) = new obinopNative(
			[](Data* av, Data* bv, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m,id);
		Value* b = bv.getValue(m,id);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(b)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				auto len = (va2.getSExtValue () );
				return new ConstantData(m.builder.CreateGlobalStringPtr(String(len,t),"tmpstr"),c_stringClass);
			}
		}
		PositionID(0,0,"!done").error("Could not find constant");
		return VOID;
	}
	,c_stringClass);

	charClass->addBinop("+",charClass) = new obinopNative(
			[](Data* av, Data* bv, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m,id);
		Value* b = bv.getValue(m,id);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(b)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				char t2 = (char)(va2.getSExtValue () );
				return new ConstantData(m.builder.CreateGlobalStringPtr(String(1,t)+String(1,t2),"tmpstr"),c_stringClass);
			}
		}
		Constant *StrConstant = ConstantDataArray::getString(getGlobalContext(), "ab");
		Module *N = (m.builder.GetInsertBlock()->getParent()->getParent());
		Module &M = *N;
		GlobalVariable *GV = new GlobalVariable(M, StrConstant->getType(),
				false, GlobalValue::PrivateLinkage,StrConstant);
		GV->setName("idk");
		GV->setUnnamedAddr(true);
		Value *Args[] = {getInt32(0),getInt32(0)};
		Value* st = m.builder.CreateInBoundsGEP(GV, Args);
		m.builder.CreateStore(a,st);
		Value *Args2[] = {getInt32(0),getInt32(1)};
		Value* st2 = m.builder.CreateInBoundsGEP(GV, Args2);
		m.builder.CreateStore(b,st2);
		return new ConstantData(st,c_stringClass);
	},c_stringClass);

	/ *c_stringClass->addBinop("+",stringClass) = new ouopNative(
				[](Data* a, RData& m) -> Data*{
					Value* len = m.builder.CreateCall(strLen, a);
					Data* str(UndefValue::get(stringClass->getType(m)));
					str= m.builder.CreateInsertValue(str,a,{0});
					str= m.builder.CreateInsertValue(str,len,{1});
					return str;
		},stringClass);*/
/*c_stringClass->addCast(stringClass) = new ouopNative(
			[](Data* av, RData& m, PositionID id) -> Data*{
		Value* a = av.getValue(m);
		Value* len = m.builder.CreateCall(strLen, a);
		Value* str = UndefValue::get(stringClass->getType(m));
		str= m.builder.CreateInsertValue(str,a,{0});
		str= m.builder.CreateInsertValue(str,len,{1});
		return new ConstantData(str,stringClass);
	},stringClass);* /
}
 */
#endif /* Declaration_HPP_ */
