/*
 * ofunction.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OFUNCTION_HPP_
#define OFUNCTION_HPP_

#include "oobjectproto.hpp"
#include "../constructs/Declaration.hpp"
#include "../constructs/Module.hpp"

class ofunction:public oobject{
	public:
		Statement* self;
		Statement* returnV;
		FunctionProto* prototype;
		Function* myFunction;
		ofunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec,RData& rd,bool add=true):oobject(id, functionClass),
				self(s),returnV(r){
			myFunction = NULL;
			prototype = new FunctionProto((self==NULL)?"unknown":(self->getMetadata(rd)->name), dec, NULL);
			if(self!=NULL){
				auto T = self->getMetadata(rd);
				if(add && T!=NULL && T->name.length()>0 && T->name[0]!='~'){
					T->funcs.add(prototype,DATA::getFunction(NULL),filePos);
									self->returnType = functionClass;
				}
			}

		}
		Constant* getConstant(RData& r) override final { return NULL; }
		String getFullName() override final{
			return prototype->name;
		}
		ReferenceElement* getMetadata(RData& r) override{
			DATA tmp = evaluate(r);
			return new ReferenceElement("",NULL,"lambda",tmp,functionClass,funcMap(std::pair<DATA, FunctionProto*>(tmp,prototype)));
		}
		void registerFunctionArgs(RData& r) override{
			if(prototype==NULL) error("Function prototype should not be null");
			if(returnV!=NULL) prototype->returnType = returnV->getSelfClass();
			for(Declaration* d: prototype->declarations){
				d->registerFunctionArgs(r);
			}
			//if(prototype->returnType==NULL) error("Function prototype-return should not be null");
		};
		void registerFunctionDefaultArgs() override{
			if(self!=NULL) self->registerFunctionDefaultArgs();
			for(Declaration* d: prototype->declarations){
				d->registerFunctionDefaultArgs();
			}
			if(returnV!=NULL) returnV->registerFunctionDefaultArgs();
		};
		void resolvePointers() override{
			if(self!=NULL) self->resolvePointers();
			for(Declaration* d: prototype->declarations){
				d->resolvePointers();
			}
			if(returnV!=NULL) returnV->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			for(auto& a:prototype->declarations){
				a->checkTypes(r);
				if(a->classV->returnType==voidClass) error("Cannot have void as parameter for function");
			}
			if(self!=NULL) self->checkTypes(r);
			if(returnV!=NULL){
				returnV->checkTypes(r);
				prototype->returnType = returnV->getSelfClass();
				if(prototype->returnType==NULL) error("Could not post-resolve return type "+returnV->returnType->name);
			} //else if(prototype->returnType==NULL) error("Could not p-resolve return type");
			return returnType;
		}
};

class externFunction : public ofunction{
	public:
		externFunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec,RData& rd):
			ofunction(id, s,r,dec,rd){
		}
		void write(ostream& f, String b) const override{
			f << "extern ";
			f << returnV << " ";
			f << (prototype->name) ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
		}
		void registerFunctionArgs(RData& a){
			if(myFunction!=NULL) return;
			ofunction::registerFunctionArgs(a);

			BasicBlock *Parent = a.builder.GetInsertBlock();
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getSelfClass()->getType(a);
				if(cl==NULL) error("Type argument "+b->classV->getFullName()+" is null");
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass();
			if(cp==NULL) error("Cannot use void class proto in extern");
			prototype->returnType = cp;
			Type* r = cp->getType(a);
			if(r==NULL) error("Type argument "+cp->name+" is null");
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, prototype->name, a.lmod);
//			Function *F = Function::Create(FT, Function::DLLImportLinkage, prototype->name, a.lmod);
			if(prototype->name=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
			else if(prototype->name=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
			else if(prototype->name=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
			else if(prototype->name=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
			else if(prototype->name=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
			else if(F->getName().str()!=prototype->name){
				a.lmod->dump();
				error("Cannot extern function due to name in use "+prototype->name +" was replaced with "+F->getName().str());
			}
			myFunction = F;
			self->getMetadata(a)->funcs.set(prototype, DATA::getFunction(F));
			if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
		}

		oobject* simplify() override final{
			return this;
		}
		DATA evaluate(RData& a) override{
			if(myFunction==NULL) registerFunctionArgs(a);
			return DATA::getFunction(myFunction);
		}
};
class lambdaFunction : public ofunction{
	public:
		Statement* ret;
		lambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, NULL,NULL,dec,rd){
			ret = r;
		}

		lambdaFunction* simplify() override final{
			return this;
/*
			std::vector<Declaration*> g;
			for(auto a:prototype->declarations) g.push_back(a->simplify());
			return new lambdaFunction(filePos,g,ret->simplify());*/
		}
		void write(ostream& f, String b) const override{
			f << "lambda ";
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,b+"  ");
			}
			f << ": ";
			f << ret;
		}
		void registerFunctionArgs(RData& r) override{
			ofunction::registerFunctionArgs(r);
			ret->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			if((prototype->returnType = ret->checkTypes(r))==NULL) error("Null prototype return");
			return prototype->returnType;
		}
		DATA evaluate(RData& ar) override{
			if(myFunction!=NULL) return DATA::getFunction(myFunction);
			if(prototype->returnType==NULL) checkTypes(ar);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getSelfClass()->getType(ar);
				if(cl==NULL) error("Type argument "+b->classV->getFullName()+" is null", true);
				args.push_back(cl);
			}
			Type* r = prototype->returnType->getType(ar);
			if(r==NULL){
				error("Error null return type for class " + prototype->returnType->name);
				r = VOIDTYPE;
			}
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::PrivateLinkage, "!lambda", ar.lmod);
			// Set names for all arguments.
			unsigned Idx = 0;
			for (Function::arg_iterator AI = F->arg_begin(); Idx != args.size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				prototype->declarations[Idx]->variable->getMetadata(ar)->setValue(AI,ar);
			}

			//BasicBlock *Parent = ar.builder.GetInsertBlock();
			//	ar.builder.SetInsertPoint(Parent);

			BasicBlock *Parent = ar.builder.GetInsertBlock();
			BasicBlock *BB =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "entry", F);
			ar.builder.SetInsertPoint(BB);
			Value* v = ret->evaluate(ar).getValue(ar);
			if(r!=VOIDTYPE)
				ar.builder.CreateRet(v);
			else
				ar.builder.CreateRetVoid();
			//cout << "testing cos" << cos(3) << endl << flush;
			verifyFunction(*F);
			//cout << "verified" << endl << flush;
			ar.fpm->run(*F);

			ar.builder.SetInsertPoint( Parent );
			return DATA::getFunction(myFunction=F);
		}
};
class userFunction : public ofunction{
	public:
		Statement* ret;
		userFunction(PositionID id, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, a, b, dec,rd){
			ret = r;
		}
		oobject* simplify() override final{
			return this;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV << " ";
			if(prototype->name.length()>0 && prototype->name[0]=='~')
				f << "operator " << prototype->name.substr(1);
			else f << (prototype->name) ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			if(ret!=NULL) ret->write(f, b);
		}
		void registerFunctionArgs(RData& ra) override{
			ofunction::registerFunctionArgs(ra);
			ret->registerFunctionArgs(ra);
			ret->registerFunctionDefaultArgs();
			ret->checkTypes(ra);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				b->registerFunctionArgs(ra);
				b->registerFunctionDefaultArgs();
				b->checkTypes(ra);
				b->classV->checkTypes(ra);
				Type* cl = b->classV->getSelfClass()->getType(ra);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata(ra)->name+" is null", true);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass();
			if(cp==NULL) error("Unknown return type");
			if(cp==autoClass){
				std::vector<ClassProto*> yields;
				ret->collectReturns(ra, yields);
				cp = getMin(yields,filePos);
				if(cp==autoClass) error("!Cannot support auto return for function");
				returnV = new ClassProtoWrapper(cp);
			}
			prototype->returnType = cp;
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::PrivateLinkage,"!"+ ((self==NULL)?"afunc":(self->getMetadata(ra)->name)), ra.lmod);
			if(self!= NULL){
				if(self->getMetadata(ra)->name.size()>0 && self->getMetadata(ra)->name[0]=='~'){
					if(prototype->declarations.size()==1){
						error("User-defined unary operators not supported");
					} else if (prototype->declarations.size()==2){
						prototype->declarations[0]->classV->getSelfClass()->addBinop(
								self->getFullName().substr(1),
								prototype->declarations[1]->classV->getSelfClass(),
								filePos
						) = new obinopUser(F, cp);
					} else error("Cannot make operator with argument count of "+str<unsigned int>(prototype->declarations.size()));
				} else self->getMetadata(ra)->funcs.set(prototype, DATA::getFunction(F));
			}
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", F);
			Jumpable* j = new Jumpable("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;

			for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				ReferenceElement* met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->llvmObject = DATA::getLocation(ra.builder.CreateAlloca(prototype->declarations[Idx]->variable->returnType->getType(ra),
						0,prototype->declarations[Idx]->variable->pointer->name));
				met->setValue(AI,ra);
			}
			ra.guarenteedReturn = false;
			ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			if(! ra.guarenteedReturn){
				if(prototype->returnType==voidClass)
					ra.builder.CreateBr(MERGE);
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			F->getBasicBlockList().remove(MERGE);
			F->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			if(r!=VOIDTYPE){
				auto functionReturnType = prototype->returnType->getType(ra);
				PHINode* phi = ra.builder.CreatePHI(functionReturnType, j->endings.size(), "funcRet" );
				for(auto &a : j->endings){
					phi->addIncoming(a.second, a.first);
				}
				ra.builder.CreateRet(phi);
			}
			else
				ra.builder.CreateRetVoid();
			free(j);
			verifyFunction(*F);
			ra.fpm->run(*F);
			myFunction = F;
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return prototype->returnType;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getFunction(myFunction);
		}
};

#define CLASSFUNC_C_
class classFunction : public ofunction{
	public:
		Statement* ret;
		ReferenceElement* thisPointer;
		String name, operation;
		classFunction(PositionID id, ReferenceElement* tPointer,String nam, String op, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, a, b, dec,rd,false),thisPointer(tPointer){
			ret = r;
			name =nam; operation = op;
			prototype->name =self->getFullName()+"."+name;
			if(name=="iterator" && prototype->declarations.size()>0) error("Cannot have arguments for iterator");
		}
		classFunction* simplify() override final{
			return this;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV << " ";
			f << (prototype->name);
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			if(ret!=NULL) ret->write(f, b);
		}
		void registerFunctionArgs(RData& ra) override{
			ofunction::registerFunctionArgs(ra);
			if(self->checkTypes(ra)!=classClass) error("Cannot define function for object whose type is not class");
			ClassProto* myCl = self->getSelfClass();
			thisPointer->returnClass = myCl;
			if(myCl==NULL) error("Defining class is null?");
			ret->registerFunctionArgs(ra);
			ret->registerFunctionDefaultArgs();
			ret->checkTypes(ra);
			if(name=="iterator"){
				myCl->iterator = this;
				return;
			}
			ReferenceElement* myMetadata = myCl->addFunction(name, filePos);
			myMetadata->funcs.add(prototype,DATA::getFunction(NULL),filePos);
			if(myMetadata==NULL) error("Metadata is null?");
			std::vector<Type*> args;
			if(myCl->isPointer) args.push_back(myCl->getType(ra));
			else args.push_back(myCl->getType(ra)->getPointerTo());
			for(auto & b: prototype->declarations){
				b->registerFunctionArgs(ra);
				b->registerFunctionDefaultArgs();
				b->checkTypes(ra);
				b->classV->checkTypes(ra);
				Type* cl = b->classV->getSelfClass()->getType(ra);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata(ra)->name+" is null", true);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass();
			if(cp==NULL) error("Unknown return type");
			if(name!="iterator" && cp==autoClass){
				std::vector<ClassProto*> yields;
				ret->collectReturns(ra, yields);
				cp = getMin(yields,filePos);
				if(cp==autoClass)  error("!Cannot support auto return for function");
				returnV = new ClassProtoWrapper(cp);
			}
			prototype->returnType = cp;
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::PrivateLinkage,"!"+self->getFullName()+"."+name, ra.lmod);
			myMetadata->funcs.set(prototype, DATA::getFunction(F));
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", F);
			Jumpable* j = new Jumpable("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = F->arg_begin();
			AI->setName("this");
			if(myCl->isPointer) thisPointer->llvmObject = DATA::getConstant(AI);
			else thisPointer->llvmObject = DATA::getLocation(AI);
			AI++;
			for (; Idx+1 != F->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				ReferenceElement* met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->llvmObject = DATA::getLocation(ra.builder.CreateAlloca(prototype->declarations[Idx]->variable->returnType->getType(ra),
						0,prototype->declarations[Idx]->variable->pointer->name));
				met->setValue(AI,ra);
			}
			ra.guarenteedReturn = false;
			ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			if( !ra.guarenteedReturn){
				if(prototype->returnType==voidClass)
					ra.builder.CreateBr(MERGE);
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			F->getBasicBlockList().remove(MERGE);
			F->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			if(r!=VOIDTYPE){
				auto functionReturnType = prototype->returnType->getType(ra);
				PHINode* phi = ra.builder.CreatePHI(functionReturnType, j->endings.size(), "funcRet" );
				for(auto &a : j->endings){
					phi->addIncoming(a.second, a.first);
				}
				ra.builder.CreateRet(phi);
			}
			else
				ra.builder.CreateRetVoid();
			free(j);
			verifyFunction(*F);
			ra.fpm->run(*F);
			myFunction = F;
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return prototype->returnType;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getFunction(myFunction);
		}
};

class constructorFunction : public ofunction{
	public:
		Statement* ret;
		ReferenceElement* thisPointer;
		constructorFunction(PositionID id, ReferenceElement* tPointer,Statement* myClass, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, NULL, myClass, dec,rd,false),thisPointer(tPointer){
			ret = r;
			prototype->name =myClass->getFullName();
		}
		constructorFunction* simplify() override final{
			return this;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV << " ";
			f << (prototype->name) << "." << prototype->name ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			if(ret!=NULL) ret->write(f, b);
		}
		void registerFunctionArgs(RData& ra) override{
			ofunction::registerFunctionArgs(ra);
			if(returnV->checkTypes(ra)!=classClass) error("Cannot define function for object whose type is not class");
			ClassProto* myCl = returnV->getSelfClass();
			if(myCl==NULL) error("Defining class is null?");
			thisPointer->returnClass = myCl;
			myCl->constructors.add(prototype,DATA::getFunction(NULL),filePos);
			ret->registerFunctionArgs(ra);
			ret->registerFunctionDefaultArgs();
			ret->checkTypes(ra);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				b->registerFunctionArgs(ra);
				b->registerFunctionDefaultArgs();
				b->checkTypes(ra);
				b->classV->checkTypes(ra);
				Type* cl = b->classV->getSelfClass()->getType(ra);
				if(cl==NULL) error("Type argument "+b->classV->getMetadata(ra)->name+" is null", true);
				args.push_back(cl);
			}
			prototype->returnType = myCl;
			Type* r = myCl->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::PrivateLinkage,"!"+returnV->getFullName(), ra.lmod);
			myCl->constructors.set(prototype, DATA::getFunction(F));
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", F);
			Jumpable* j = new Jumpable("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = F->arg_begin();
			for (; Idx != F->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				auto met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->llvmObject = DATA::getLocation(ra.builder.CreateAlloca(prototype->declarations[Idx]->variable->returnType->getType(ra),
						0,prototype->declarations[Idx]->variable->pointer->name));
				met->setValue(AI,ra);
			}
			thisPointer->llvmObject = DATA::getLocation(ra.builder.CreateAlloca(r, 0,"this*"));
			thisPointer->setValue(myCl->generateData(ra),ra);
			ra.guarenteedReturn = false;
			ret->evaluate(ra);
			if(ra.popJump()!=j) error("Did not receive same func jumpable created");
			if(ra.guarenteedReturn)  error("Can not have return statement in constructor");
			ra.builder.CreateBr(MERGE);
			ra.guarenteedReturn = false;

			F->getBasicBlockList().remove(MERGE);
			F->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			ra.builder.CreateRet(thisPointer->getValue(ra));
			free(j);
			verifyFunction(*F);
			ra.fpm->run(*F);
			myFunction = F;
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return prototype->returnType;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getFunction(myFunction);
		}
};
#endif /* OFUNCTION_HPP_ */
