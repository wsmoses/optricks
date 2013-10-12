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
		//TODO check
		bool hasValue() const {
			return value!=NULL;
		}
		const Token getToken() const final override{
			return T_DECLARATION;
		}
		ClassProto* checkTypes(RData& r) final override{
			classV->checkTypes(r);
			variable->checkTypes(r);
			//cout << "checking declaration types: "<<(size_t)(value) << " and now the class" <<
				//	((value==NULL)?c_intClass:(value->returnType))->name << endl << flush;
			if(value!=NULL){
				value->checkTypes(r);
				if(value->returnType==NULL)error("Declaration of inconsistent types");
				else if(classV->getSelfClass()==autoClass){
					variable->getMetadata(r)->returnClass = variable->returnType = value->returnType;
					//cout << "Location of set" << (size_t)(variable->getMetadata(r)) << endl << flush;
					//cout << "Setting: " << variable->returnType->name << endl << flush;
					classV = new ClassProtoWrapper(variable->returnType);
					//cout << "Finalized as: " << variable->checkTypes(r)->name << endl << flush;
				}
				else if(!value->returnType->hasCast(classV->getSelfClass()) )
					error("Declaration of inconsistent types - variable of type "+classV->getFullName()+" and value of "+value->returnType->name);
			}
			return returnType;
		}

		void registerClasses(RData& r) override final{
			classV->registerClasses(r);
			variable->registerClasses(r);
			if(value!=NULL) value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			classV->registerFunctionArgs(r);
			variable->registerFunctionArgs(r);
			if(value!=NULL) value->registerFunctionArgs(r);
			classV->checkTypes(r);
			if(classV->getSelfClass()==NULL) error("Argument " + classV->getFullName() + "is not a class DC");
			variable->getMetadata(r)->returnClass = variable->returnType = classV->getMetadata(r)->selfClass;
			variable->checkTypes(r);
			//checkTypes(r);
			//TODO add name in table of args?
		};
		void registerFunctionDefaultArgs() override final{
			classV->registerFunctionDefaultArgs();
			variable->registerFunctionDefaultArgs();
			if(value!=NULL) value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			classV->resolvePointers();
			variable->resolvePointers();
			if(value!=NULL) value->resolvePointers();
		};
		DATA evaluate(RData& r) final override{
			//cout << "evaling declaration " << endl << flush;
			checkTypes(r);
			if(global){

				Module *N = (r.builder.GetInsertBlock()->getParent()->getParent());
				Module &M = *N;
				//TODO introduce constant expressions
				//Constant* cons = getConstant(r);
				//(tmp==NULL)?NULL:dynamic_cast<Constant*>(tmp);
				DATA tmp = (value==NULL || value->getToken()==T_VOID)?NULL:(value->returnType->castTo(r, value->evaluate(r), variable->returnType));

				GlobalVariable *GV = new GlobalVariable(M, variable->returnType->getType(r),
						false, GlobalValue::PrivateLinkage,UndefValue::get(variable->returnType->getType(r)));
				GV->setName(variable->getFullName());
				//if(cons==NULL && tmp!=NULL){
				r.builder.CreateStore(tmp,GV);
				//}
				variable->getMetadata(r)->llvmLocation = Alloca = GV;
			}
			else{
				Function *TheFunction = r.builder.GetInsertBlock()->getParent();
				IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
						TheFunction->getEntryBlock().begin());
				Alloca = TmpB.CreateAlloca(variable->returnType->getType(r), 0,variable->pointer->name);
				if(value!=NULL && value->getToken()!=T_VOID){
					r.builder.CreateStore(value->returnType->castTo(r, value->evaluate(r), variable->returnType) , Alloca);
				}
			}
			variable->getMetadata(r)->llvmLocation = Alloca;
			r.guarenteedReturn = false;
			return NULL;
			//variable->pointer->resolve() = r.builder.CreateLoad(Alloca);
			//			error("Todo: allow declaration evaluation");
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

std::pair<bool,std::pair<unsigned int, unsigned int>> FunctionProto::match(FunctionProto* func) const{
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
		ClassProto* class1 = declarations[a]->classV->getSelfClass();
		ClassProto* class2 = func->declarations[a]->classV->getSelfClass();
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

bool FunctionProto::equals(const FunctionProto* f) const{
	if(declarations.size()!=f->declarations.size()) return false;
	for(unsigned int i = 0; i<declarations.size(); ++i){
		ClassProto* class1 = declarations[i]->classV->getSelfClass();
		ClassProto* class2 = f->declarations[i]->classV->getSelfClass();
		if(class1==NULL || class2==NULL) todo("ERROR: NULL PROTO",PositionID());
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

Function* strLen;
void initFuncsMeta(RData& rd){
	//cout << "SIZE OF C_STR: " <<rd.lmod->getPointerSize() << endl << flush;
	//cout << "SIZE OF ANY: " << Module::PointerSize::AnyPointerSize << endl << flush;
	//cout << "SIZE OF x32: " << Module::PointerSize::Pointer32 << endl << flush;
	//cout << "SIZE OF x64: " << Module::PointerSize::Pointer64 << endl << flush;
	//cout << "SIZE OF PTR: " << sizeof(char*) << endl << flush;
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
		intClass->constructors.add(intIntP,F,PositionID());
	}
	{
		std::vector<Type*> t = {C_STRINGTYPE};
		FunctionType *FT = FunctionType::get(INTTYPE, t, false);
		strLen = Function::Create(FT, Function::ExternalLinkage, "strlen",rd.lmod);
		rd.exec->addGlobalMapping(strLen, (void*)(&strlen));
	}
	c_stringClass->addFunction("length",PositionID())->funcs.add(new FunctionProto("length",intClass),strLen,PositionID());
	{

		std::vector<Type*> args = {CHARTYPE->getPointerTo(0)};
		FunctionType *FT = FunctionType::get(INTTYPE, args, false);
		Function *F = Function::Create(FT, Function::PrivateLinkage,"!return1", rd.lmod);
		BasicBlock *Parent = rd.builder.GetInsertBlock();
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
		rd.builder.SetInsertPoint(BB);
		rd.builder.CreateRet(ConstantInt::get(INTTYPE,1));
		if(Parent!=NULL) rd.builder.SetInsertPoint(Parent);
		charClass->addFunction("length",PositionID())->funcs.add(new FunctionProto("length",intClass),F,PositionID());
	}
	charClass->addCast(stringClass) = new ouopNative(
			[](DATA a, RData& m) -> DATA{
		DATA str = UndefValue::get(stringClass->getType(m));
		Constant *StrConstant = ConstantDataArray::getString(getGlobalContext(), "a");
		Module *N = (m.builder.GetInsertBlock()->getParent()->getParent());
		Module &M = *N;
		GlobalVariable *GV = new GlobalVariable(M, StrConstant->getType(),
				true, GlobalValue::PrivateLinkage,StrConstant);
		GV->setName("idk");
		GV->setUnnamedAddr(true);
		Value *Args[] = {getInt32(0),getInt32(0)};

		DATA st = m.builder.CreateInBoundsGEP(GV, Args);

		m.builder.CreateStore(a,st);

		str= m.builder.CreateInsertValue(str,st,{0});
		str= m.builder.CreateInsertValue(str,getInt(1),{1});
		return str;
	},stringClass);
	/*c_stringClass->addBinop("+",c_stringClass) = new obinopNative(
						[](DATA a, DATA b, RData& m) -> DATA{
							Constant *StrConstant = ConstantDataArray::getString(getGlobalContext(), "ab");
							Module *N = (m.builder.GetInsertBlock()->getParent()->getParent());
							Module &M = *N;
							GlobalVariable *GV = new GlobalVariable(M, StrConstant->getType(),
							true, GlobalValue::PrivateLinkage,StrConstant);
							GV->setName("idk");
							GV->setUnnamedAddr(true);
							Value *Args[] = {getInt32(0),getInt32(0)};
							DATA st = m.builder.CreateInBoundsGEP(GV, Args);
							m.builder.CreateStore(a,st);
							Value *Args2[] = {getInt32(0),getInt32(1)};
							DATA st2 = m.builder.CreateInBoundsGEP(GV, Args2);
							m.builder.CreateStore(b,st2);
							return st;
				},c_stringClass);*/
	charClass->addBinop("+",charClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
		Constant *StrConstant = ConstantDataArray::getString(getGlobalContext(), "ab");
		Module *N = (m.builder.GetInsertBlock()->getParent()->getParent());
		Module &M = *N;
		GlobalVariable *GV = new GlobalVariable(M, StrConstant->getType(),
				true, GlobalValue::PrivateLinkage,StrConstant);
		GV->setName("idk");
		GV->setUnnamedAddr(true);
		Value *Args[] = {getInt32(0),getInt32(0)};
		DATA st = m.builder.CreateInBoundsGEP(GV, Args);
		m.builder.CreateStore(a,st);
		Value *Args2[] = {getInt32(0),getInt32(1)};
		DATA st2 = m.builder.CreateInBoundsGEP(GV, Args2);
		m.builder.CreateStore(b,st2);
		return st;
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
			[](DATA a, RData& m) -> DATA{
		Value* len = m.builder.CreateCall(strLen, a);
		DATA str = UndefValue::get(stringClass->getType(m));
		str= m.builder.CreateInsertValue(str,a,{0});
		str= m.builder.CreateInsertValue(str,len,{1});
		return str;
	},stringClass);
}
#endif /* Declaration_HPP_ */
