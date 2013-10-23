/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../constructs/Statement.hpp"
#include "../expressions/E_VAR.hpp"

#define DECLR_P_
class Declaration: public Construct{
	public:
		Statement* classV;
		E_VAR* variable;
		Statement* value;
		Value* Alloca;
		bool global = false;
		Declaration(PositionID id, Statement* v, E_VAR* loc, bool glob, Statement* e) : Construct(id, voidClass){
			classV = v;
			variable = loc;
			value = e;
			global = glob;
			Alloca = NULL;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
		bool hasValue() const {
			return value!=NULL && value->getToken()!=T_VOID;
		}
		const Token getToken() const final override{
			return T_DECLARATION;
		}
		ClassProto* checkTypes(RData& r) final override{
			classV->checkTypes(r);
			returnType = classV->getSelfClass(r);
			//cout << "checking declaration types: "<<(size_t)(value) << " and now the class" <<
			//	((value==NULL)?c_intClass:(value->returnType))->name << endl << flush;
			if(value!=NULL){
				value->checkTypes(r);
				if(value->returnType==NULL)error("Declaration of inconsistent types");
				else if(returnType==autoClass){
					returnType = value->returnType;
					//cout << "Location of set" << (size_t)(variable->getMetadata(r)) << endl << flush;
					//cout << "Setting: " << variable->returnType->name << endl << flush;
					classV = new ClassProtoWrapper(returnType);
					//cout << "Finalized as: " << variable->checkTypes(r)->name << endl << flush;
				}
				else if(!value->returnType->hasCast(returnType) )
					error("Declaration of inconsistent types - variable of type "+classV->getFullName()+" and value of "+value->returnType->name);
			}
			variable->getMetadata(r)->returnClass = variable->returnType = returnType;
			//variable->checkTypes(r);
			return returnType;
		}

		void registerClasses(RData& r) override final{
			if(classV!=NULL) classV->registerClasses(r);
			if(variable!=NULL) variable->registerClasses(r);
			if(value!=NULL) value->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			if(classV!=NULL) classV->registerFunctionPrototype(r);
			if(variable!=NULL) variable->registerFunctionPrototype(r);
			if(value!=NULL) value->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) override final{
			if(classV!=NULL) classV->buildFunction(r);
			if(variable!=NULL) variable->buildFunction(r);
			if(value!=NULL) value->buildFunction(r);
		};
		void resolvePointers() override final{
			if(classV!=NULL) classV->resolvePointers();
			if(classV!=NULL) variable->resolvePointers();
			if(value!=NULL) value->resolvePointers();
		};
		DATA evaluate(RData& r) final override{
			checkTypes(r);
			assert(variable->returnType!=voidClass);
			if(global){

				Module *N = (r.builder.GetInsertBlock()->getParent()->getParent());
				Module &M = *N;
				//TODO introduce constant expressions
				//Constant* cons = getConstant(r);
				//(tmp==NULL)?NULL:dynamic_cast<Constant*>(tmp);
				Value* tmp = (value==NULL || value->getToken()==T_VOID)?NULL:(value->evaluate(r).castTo(r, variable->returnType, filePos).getValue(r));

				GlobalVariable *GV = new GlobalVariable(M, variable->returnType->getType(r),
						false, GlobalValue::PrivateLinkage,UndefValue::get(variable->returnType->getType(r)));
				GV->setName(variable->getFullName());
				//if(cons==NULL && tmp!=NULL){
				r.builder.CreateStore(tmp,GV);
				//}
				Alloca = GV;
			}
			else{
				Function *TheFunction = r.builder.GetInsertBlock()->getParent();
				IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
						TheFunction->getEntryBlock().begin());
				Alloca = TmpB.CreateAlloca(variable->returnType->getType(r), 0,variable->pointer->name);
				if(value!=NULL && value->getToken()!=T_VOID){
					r.builder.CreateStore(value->evaluate(r).castTo(r, variable->returnType, filePos).getValue(r) , Alloca);
				}
			}
			variable->getMetadata(r)->llvmObject = DATA::getLocation(Alloca,variable->returnType);
			r.guarenteedReturn = false;
			return DATA::getNull();
		}
		Declaration* simplify() final override{
			return new Declaration(filePos, classV, variable, global, (value==NULL)?NULL:(value->simplify()));
		}
		void write(ostream& f, String s="") const final override{
			//f << "d(";
			if(classV!=NULL) f << classV << " ";
			else f << "auto ";
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
			//f << ")";
		}
};

std::pair<bool,std::pair<unsigned int, unsigned int>> FunctionProto::match(RData& r, FunctionProto* func) const{
	unsigned int optional;
	if(func->declarations.size()!=declarations.size()){
		if(declarations.size()>func->declarations.size()) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		for(unsigned int a=declarations.size(); a<func->declarations.size(); ++a){
			if(!func->declarations[a]->hasValue()) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		}
		optional = func->declarations.size()-declarations.size();
	} else optional = 0;
	unsigned int count=0;
	for(unsigned int a=0; a<declarations.size(); ++a){
		ClassProto* class1 = declarations[a]->classV->getSelfClass(r);
		ClassProto* class2 = func->declarations[a]->classV->getSelfClass(r);
		if(class1==voidClass){
			if(!func->declarations[a]->hasValue())
				return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		}
		else{
			auto t = class1->compatable(class2);
			if(!t.first)  return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
			else{
				if(t.second>0) count++;
			}
		}
	}
	return std::pair<bool,std::pair<unsigned int, unsigned int> >(true,std::pair<unsigned int, unsigned int>(optional,count));
}

bool FunctionProto::equals(RData& r, const FunctionProto* f, PositionID id) const{
	if(declarations.size()!=f->declarations.size()) return false;
	for(unsigned int i = 0; i<declarations.size(); ++i){
		ClassProto* class1 = declarations[i]->classV->getSelfClass(r);
		ClassProto* class2 = f->declarations[i]->classV->getSelfClass(r);
		if(class1==NULL || class2==NULL) id.error("ERROR: NULL PROTO");
		if(!class1->equals(class2))
			return false;
	}
	return true;
}
String FunctionProto::toString() const{
	String t = name+"(";
	bool first = true;
	for(const auto& a: declarations){
		if(first){
			first = false;
		} else t+=",";
		t+=a->classV->getFullName();
	}
	return t+")";
}

ClassProto* FunctionProto::getGeneratorType(RData& r, PositionID id){
	if(generatorType!=NULL) return generatorType;
	generatorType = new ClassProto(objectClass,name);
	assert(generatorType!=NULL);
	generatorType->isGen = true;
	for(const auto& a: declarations){
		generatorType->addElement(a->variable->getFullName(),a->classV->getSelfClass(r),id);
	}
	return generatorType;
}

Function* strLen;
void initFuncsMeta(RData& rd){
	//cout << "SIZE OF C_STR: " <<rd.lmod->getPointerSize() << endl << flush;
	//cout << "SIZE OF ANY: " << Module::PointerSize::AnyPointerSize << endl << flush;
	//cout << "SIZE OF x32: " << Module::PointerSize::Pointer32 << endl << flush;
	//cout << "SIZE OF x64: " << Module::PointerSize::Pointer64 << endl << flush;
	//cout << "SIZE OF PTR: " << sizeof(char*) << endl << flush;
	//TODO begin conversion of constructors to generators
	{
		FunctionProto* intIntP = new FunctionProto("int",intClass);
		intIntP->declarations.push_back(new Declaration(PositionID(),new ClassProtoWrapper(doubleClass),NULL,false,NULL));

		std::vector<Type*> args = {DOUBLETYPE};
		FunctionType *FT = FunctionType::get(INTTYPE, args, false);
		Function *F = Function::Create(FT, Function::PrivateLinkage,"!int", rd.lmod);
		BasicBlock *Parent = rd.builder.GetInsertBlock();
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		rd.builder.SetInsertPoint(BB);
		rd.builder.CreateRet(rd.builder.CreateFPToSI(F->arg_begin(), INTTYPE));
		if(Parent!=NULL) rd.builder.SetInsertPoint(Parent);
		intClass->constructors.add(DATA::getFunction(F,intIntP),rd, PositionID());
	}
	{
		std::vector<Type*> t = {C_STRINGTYPE};
		FunctionType *FT = FunctionType::get(INTTYPE, t, false);
		strLen = Function::Create(FT, Function::ExternalLinkage, "strlen",rd.lmod);
		rd.exec->addGlobalMapping(strLen, (void*)(&strlen));
	}
	c_stringClass->addFunction("length",PositionID())->funcs.add(DATA::getFunction(strLen,new FunctionProto("length", intClass)),rd, PositionID());
	{

		std::vector<Type*> args = {CHARTYPE->getPointerTo(0)};
		FunctionType *FT = FunctionType::get(INTTYPE, args, false);
		Function *F = Function::Create(FT, Function::PrivateLinkage,"!return1", rd.lmod);
		BasicBlock *Parent = rd.builder.GetInsertBlock();
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		rd.builder.SetInsertPoint(BB);
		rd.builder.CreateRet(ConstantInt::get(INTTYPE,1));
		if(Parent!=NULL) rd.builder.SetInsertPoint(Parent);
		charClass->addFunction("length",PositionID())->funcs.add(DATA::getFunction(F,new FunctionProto("length",intClass)),rd, PositionID());
	}
	charClass->addCast(c_stringClass) = new ouopNative(
		[](DATA av, RData& m, PositionID id) -> DATA{
	Value* a = av.getValue(m);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			APInt va = constantInt->getValue();
			char t = (char)(va.getSExtValue () );
			return DATA::getConstant(m.builder.CreateGlobalStringPtr(String(1,t),"tmpstr"),c_stringClass);
		}
		id.error("Cannot convert from non-const char to string");
		return DATA::getNull();
	},c_stringClass);
	charClass->addCast(stringClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		if(ConstantInt* c = dyn_cast<ConstantInt>(a)){
			auto va = c->getValue();
			char t = (char)(va.getSExtValue () );
			Value* st = m.builder.CreateGlobalStringPtr(String(1,t),"tmpstr");
			Value* str = UndefValue::get(stringClass->getType(m));
			str= m.builder.CreateInsertValue(str,st,{0});
			str= m.builder.CreateInsertValue(str,getInt(1),{1});
			return DATA::getConstant(str,stringClass);
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
		return DATA::getConstant(str,stringClass);
	},stringClass);
	//c_stringClass->addBinop("+",c_stringClass) = new obinopNative(
	//		[](Value* ay, Value* by, RData& m) -> DATA{
		//TODO string addition (need malloc)
	//},c_stringClass);

	intClass->addBinop("*",charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		Value* b = bv.getValue(m);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(b)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(a)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				auto len = (va2.getSExtValue () );
				return DATA::getConstant(m.builder.CreateGlobalStringPtr(String(len,t),"tmpstr"),c_stringClass);
			}
		}
		PositionID(0,0,"!done").error("Could not find constant");
		return DATA::getNull();
		}
	,c_stringClass);
	charClass->addBinop("*",intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		Value* b = bv.getValue(m);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(b)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				auto len = (va2.getSExtValue () );
				return DATA::getConstant(m.builder.CreateGlobalStringPtr(String(len,t),"tmpstr"),c_stringClass);
			}
		}
		PositionID(0,0,"!done").error("Could not find constant");
		return DATA::getNull();
		}
	,c_stringClass);

	charClass->addBinop("+",charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		Value* b = bv.getValue(m);
		if(ConstantInt* constantInt = dyn_cast<ConstantInt>(a)){
			if(ConstantInt* cons2 = dyn_cast<ConstantInt>(b)){
				APInt va = constantInt->getValue();
				char t = (char)(va.getSExtValue () );
				APInt va2 = cons2->getValue();
				char t2 = (char)(va2.getSExtValue () );
				return DATA::getConstant(m.builder.CreateGlobalStringPtr(String(1,t)+String(1,t2),"tmpstr"),c_stringClass);
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
		return DATA::getConstant(st,c_stringClass);
	},c_stringClass);

	/*c_stringClass->addBinop("+",stringClass) = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					Value* len = m.builder.CreateCall(strLen, a);
					DATA str = UndefValue::get(stringClass->getType(m));
					str= m.builder.CreateInsertValue(str,a,{0});
					str= m.builder.CreateInsertValue(str,len,{1});
					return str;
		},stringClass);*/
	c_stringClass->addCast(stringClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		Value* len = m.builder.CreateCall(strLen, a);
		Value* str = UndefValue::get(stringClass->getType(m));
		str= m.builder.CreateInsertValue(str,a,{0});
		str= m.builder.CreateInsertValue(str,len,{1});
		return DATA::getConstant(str,stringClass);
	},stringClass);
}
#endif /* Declaration_HPP_ */
