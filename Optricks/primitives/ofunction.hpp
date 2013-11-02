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
		bool built;
		ofunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec,RData& rd,bool add=true):oobject(id, functionClass),
				self(s),returnV(r),built(false){
			myFunction = NULL;
			prototype = new FunctionProto((self==NULL)?"unknown":(self->getMetadata(rd)->name), dec, NULL);
		}
		String getFullName() override final{
			return prototype->name;
		}
		ReferenceElement* getMetadata(RData& r) override{
			DATA tmp = evaluate(r);
			return new ReferenceElement("",NULL,"lambda",tmp,funcMap(tmp));
		}
		void registerFunctionPrototype(RData& r) override{
			if(self!=NULL){
				self->registerFunctionPrototype(r);
			}
			if(returnV!=NULL){
				returnV->registerFunctionPrototype(r);
				prototype->returnType = returnV->getSelfClass(r);
			}
			for(Declaration* d: prototype->declarations){
				d->registerFunctionPrototype(r);
			}
			//for(Declaration* d: prototype->declarations){
			//	d->classV
			//}
		};
		void buildFunction(RData& r) override{
			if(myFunction==NULL) registerFunctionPrototype(r);
			if(self!=NULL) self->buildFunction(r);
			if(returnV!=NULL) returnV->buildFunction(r);
			for(Declaration* d: prototype->declarations){
				d->buildFunction(r);
			}
			built = true;
		};
		DATA evaluate(RData& a) override{
			if(!built) buildFunction(a);
			return DATA::getFunction(myFunction,prototype);
		}
		ClassProto* checkTypes(RData& r) override{
			for(auto& a:prototype->declarations){
				a->checkTypes(r);
				if(a->classV->returnType==voidClass) error("Cannot have void as parameter for function");
			}
			//if(self!=NULL) self->checkTypes(r);
			if(returnV!=NULL){
				//returnV->checkTypes(r);
				prototype->returnType = returnV->getSelfClass(r);
				assert(prototype->returnType!=NULL);// error("Could not post-resolve return type "+returnV->returnType->name);
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
			f << returnV->getFullName() << " ";
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
		void registerFunctionPrototype(RData& a){
			if(myFunction!=NULL) return;
			ofunction::registerFunctionPrototype(a);

			BasicBlock *Parent = a.builder.GetInsertBlock();
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getSelfClass(a)->getType(a);
				if(cl==NULL) error("Type argument "+b->classV->getFullName()+" is null");
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass(a);
			assert(cp!=NULL);
			prototype->returnType = cp;
			Type* r = cp->getType(a);
			assert(r!=NULL);
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, prototype->name, a.lmod);
			if(prototype->name=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
			else if(prototype->name=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
			else if(prototype->name=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
			else if(prototype->name=="printby") a.exec->addGlobalMapping(F, (void*)(&printby));
			else if(prototype->name=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
			else if(prototype->name=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
			else if(F->getName().str()!=prototype->name){
				error("Cannot extern function due to name in use "+prototype->name +" was replaced with "+F->getName().str());
			}
			myFunction = F;
			self->getMetadata(a)->funcs.set(DATA::getFunction(F,prototype), a, filePos);
			if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
		}
};
class lambdaFunction : public ofunction{
	public:
		Statement* ret;
		lambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, NULL,NULL,dec,rd){
			ret = r;
		}
		const Token getToken() const override final{
			return T_LAMBDAFUNC;
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
		void registerFunctionPrototype(RData& r) override{
			ofunction::registerFunctionPrototype(r);
			ret->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) override final{
			ofunction::buildFunction(r);
			ret->buildFunction(r);
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			if((prototype->returnType = ret->checkTypes(r))==NULL) error("Null prototype return");
			return returnType;
		}
		DATA evaluate(RData& ar) override{
			if(myFunction!=NULL) return DATA::getFunction(myFunction,prototype);
			if(prototype->returnType==NULL) checkTypes(ar);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getSelfClass(ar)->getType(ar);
				assert(cl!=NULL);
				args.push_back(cl);
			}
			Type* r = prototype->returnType->getType(ar);
			assert(r!=NULL);
			FunctionType *FT = FunctionType::get(r, args, false);
			myFunction = Function::Create(FT, Function::PrivateLinkage, "!lambda", ar.lmod);
			// Set names for all arguments.
			unsigned Idx = 0;
			for (Function::arg_iterator AI = myFunction->arg_begin(); Idx != args.size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				prototype->declarations[Idx]->variable->getMetadata(ar)->setValue(AI,ar);
			}

			BasicBlock *Parent = ar.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", myFunction);
			ar.builder.SetInsertPoint(BB);
			DATA dat = ret->evaluate(ar);
			if(
					//dat.getReturnType(ar, filePos)!=voidClass
					//TODO check this
					dat.getType()!=R_UNDEF) ar.builder.CreateRet(dat.getValue(ar));
			else ar.builder.CreateRetVoid();
			verifyFunction(*myFunction);
			ar.fpm->run(*myFunction);
			ar.builder.SetInsertPoint( Parent );
			return DATA::getFunction(myFunction,prototype);
		}
};
class userFunction : public ofunction{
	public:
		Statement* ret;
		userFunction(PositionID id, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, a, b, dec,rd){
			ret = r;
		}
		void write(ostream& f, String b) const override{
			f << "def " << flush;
			if(returnV!=NULL) f << returnV->getFullName() << " ";
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
		void registerFunctionPrototype(RData& ra) override{
			ofunction::registerFunctionPrototype(ra);
			std::vector<Type*> args;
			for(Declaration*& b: prototype->declarations){
				b->registerFunctionPrototype(ra);
				ClassProto* cla = b->checkTypes(ra);
				Type* cl = cla->getType(ra);
				assert(cl!=NULL);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass(ra);
			assert(cp!=NULL);
			if(cp==autoClass){
				std::vector<ClassProto*> yields;
				ret->collectReturns(ra, yields,autoClass);
				cp = getMin(yields,filePos);
				if(cp==autoClass) error("Cannot deduce return type of function "+getFullName());
				returnV = new ClassProtoWrapper(cp);
			} else if(returnV->getToken()!=T_CLASSPROTO) returnV = new ClassProtoWrapper(cp);
			prototype->returnType = cp;
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			myFunction = Function::Create(FT, Function::PrivateLinkage,"!"+ ((self==NULL)?"afunc":(self->getMetadata(ra)->name)), ra.lmod);
			if(self!= NULL){
				if(self->getMetadata(ra)->name.size()>0 && self->getMetadata(ra)->name[0]=='~'){
					if(prototype->declarations.size()==1){
						error("User-defined unary operators not supported");
					} else if (prototype->declarations.size()==2){
						prototype->declarations[0]->classV->getSelfClass(ra)->addBinop(
								self->getFullName().substr(1),
								prototype->declarations[1]->classV->getSelfClass(ra),
								filePos
						) = new obinopUser(myFunction, cp);
					} else error("Cannot make operator with argument count of "+str<unsigned int>(prototype->declarations.size()));
				} else self->getMetadata(ra)->funcs.set(DATA::getFunction(myFunction,prototype), ra, filePos);
			}
			ret->registerFunctionPrototype(ra);
		};
		void buildFunction(RData& ra) override final{
			ofunction::buildFunction(ra);
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", myFunction);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", myFunction);
			Jumpable j("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(&j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;

			for (Function::arg_iterator AI = myFunction->arg_begin(); Idx != myFunction->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				ReferenceElement* met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->setObject(DATA::getConstant(AI,prototype->declarations[Idx]->variable->returnType).toLocation(ra));
			}
			ra.guarenteedReturn = false;
			ret->checkTypes(ra);
			ret->evaluate(ra);
			if(ra.popJump()!=&j) error("Did not receive same func jumpable created");
			if(! ra.guarenteedReturn){
				if(prototype->returnType==voidClass)
					ra.builder.CreateBr(MERGE);
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			myFunction->getBasicBlockList().remove(MERGE);
			myFunction->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			if(prototype->returnType!=voidClass){
				Type* functionReturnType = prototype->returnType->getType(ra);
				PHINode* phi = ra.builder.CreatePHI(functionReturnType, j.endings.size(), "funcRetU" );
				for(auto &a : j.endings){
					phi->addIncoming(a.second.getValue(ra), a.first);
				}
				ra.builder.CreateRet(phi);
			}
			else
				ra.builder.CreateRetVoid();
			verifyFunction(*myFunction);
			ra.fpm->run(*myFunction);
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
			ret->buildFunction(ra);
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return returnType;
		}
};

#define CLASSFUNC_C_
class classFunction : public ofunction{
	public:
		Statement* ret;
		ReferenceElement* thisPointer;
		String name;
		ClassProto* upperClass;
		classFunction(PositionID id, ReferenceElement* tPointer,String nam, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, a, b, dec,rd,false),thisPointer(tPointer){
			ret = r;
			name =nam;
			prototype->name =self->getFullName()+"."+name;
			upperClass = NULL;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV->getFullName() << " ";
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
		void registerFunctionPrototype(RData& ra) override{
			ofunction::registerFunctionPrototype(ra);
			upperClass = self->getSelfClass(ra);
			assert(upperClass!=NULL);
			ReferenceElement* myMetadata = upperClass->addFunction(name, filePos);
			myMetadata->funcs.add(DATA::getFunction(NULL,prototype),ra,filePos);
			assert(myMetadata!=NULL);
			std::vector<Type*> args;
			if(upperClass->layoutType==POINTER_LAYOUT || upperClass->layoutType==PRIMITIVEPOINTER_LAYOUT)
				args.push_back(upperClass->getType(ra));
			else args.push_back(upperClass->getType(ra)->getPointerTo());
			for(auto & b: prototype->declarations){
				b->registerFunctionPrototype(ra);
				ClassProto* cla = b->checkTypes(ra);
				Type* cl = cla->getType(ra);
				assert(cl!=NULL);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getSelfClass(ra);
			if(cp==NULL) error("Unknown return type");
			if(cp==autoClass){
				std::vector<ClassProto*> yields;
				ret->collectReturns(ra, yields,autoClass);
				cp = getMin(yields,filePos);
				if(cp==autoClass)  error("!Cannot support auto return for function");
				returnV = new ClassProtoWrapper(cp);
			} else if(returnV->getToken()!=T_CLASSPROTO) returnV = new ClassProtoWrapper(cp);
			prototype->returnType = cp;
			Type* r = cp->getType(ra);

			FunctionType *FT = FunctionType::get(r, args, false);
			myFunction = Function::Create(FT, Function::PrivateLinkage,"!"+self->getFullName()+"."+name, ra.lmod);
			myMetadata->funcs.set(DATA::getFunction(myFunction,prototype), ra, filePos);
			ret->registerFunctionPrototype(ra);
		};
		void buildFunction(RData& ra) override final{
			ofunction::buildFunction(ra);
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", myFunction);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", myFunction);
			Jumpable j("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(&j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = myFunction->arg_begin();
			AI->setName("this");
			if(upperClass->layoutType==POINTER_LAYOUT || upperClass->layoutType==PRIMITIVEPOINTER_LAYOUT) thisPointer->setObject(DATA::getConstant(AI, upperClass));
			else thisPointer->setObject(DATA::getLocation(AI, upperClass));
			AI++;
			for (; Idx+1 != myFunction->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				ReferenceElement* met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->setObject(DATA::getConstant(AI,prototype->declarations[Idx]->variable->returnType).toLocation(ra));
			}
			ra.guarenteedReturn = false;
			ret->checkTypes(ra);
			ret->evaluate(ra);
			if(ra.popJump()!= &j) error("Did not receive same func jumpable created");
			if( !ra.guarenteedReturn){
				if(prototype->returnType==voidClass)
					ra.builder.CreateBr(MERGE);
				else error("Could not find return statement");
			}
			ra.guarenteedReturn = false;

			myFunction->getBasicBlockList().remove(MERGE);
			myFunction->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			if(prototype->returnType!=voidClass){
				auto functionReturnType = prototype->returnType->getType(ra);
				PHINode* phi = ra.builder.CreatePHI(functionReturnType, j.endings.size(), "funcRetCl" );
				for(auto &a : j.endings){
					phi->addIncoming(a.second.getValue(ra), a.first);
				}
				ra.builder.CreateRet(phi);
			}
			else
				ra.builder.CreateRetVoid();
			verifyFunction(*myFunction);
			ra.fpm->run(*myFunction);
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
			ret->buildFunction(ra);
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return returnType;
		}
};

class constructorFunction : public ofunction{
	public:
		Statement* ret;
		ReferenceElement* thisPointer;
		ClassProto* upperClass;
		constructorFunction(PositionID id, ReferenceElement* tPointer,Statement* myClass, std::vector<Declaration*> dec, Statement* r,RData& rd):
			ofunction(id, NULL, myClass, dec,rd,false),thisPointer(tPointer){
			ret = r;
			prototype->name =myClass->getFullName();
			upperClass = NULL;
		}
		void write(ostream& f, String b) const override{
			f << "def ";
			f << returnV->getFullName() << " ";
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
		void registerFunctionPrototype(RData& ra) override{
			ofunction::registerFunctionPrototype(ra);
			upperClass = returnV->getSelfClass(ra);
			assert(upperClass!=NULL);
			upperClass->constructors.add(DATA::getFunction(NULL,prototype),ra,filePos);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				b->registerFunctionPrototype(ra);
				ClassProto* cla = b->checkTypes(ra);
				Type* cl = cla->getType(ra);
				assert(cl!=NULL);
				args.push_back(cl);
			}
			Type* r = upperClass->getType(ra);
			FunctionType *FT = FunctionType::get(r, args, false);
			myFunction = Function::Create(FT, Function::PrivateLinkage,"!"+returnV->getFullName(), ra.lmod);
			upperClass->constructors.set(DATA::getFunction(myFunction,prototype), ra, filePos);
			ret->registerFunctionPrototype(ra);
		};
		void buildFunction(RData& ra) override final{
			ofunction::buildFunction(ra);
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", myFunction);
			BasicBlock *MERGE = BasicBlock::Create(getGlobalContext(), "funcMerge", myFunction);
			Jumpable j = Jumpable("", FUNC, NULL, MERGE, prototype->returnType);
			ra.addJump(&j);
			ra.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = myFunction->arg_begin();
			for (; Idx != myFunction->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				auto met = prototype->declarations[Idx]->variable->getMetadata(ra);
				met->setObject(DATA::getConstant(AI, prototype->declarations[Idx]->variable->returnType));
			}
			thisPointer->setObject(DATA::getLocation(ra.builder.CreateAlloca(upperClass->getType(ra), 0,"this*"), upperClass));
			thisPointer->setValue(upperClass->generateData(ra),ra);
			ra.guarenteedReturn = false;
			ret->checkTypes(ra);
			ret->evaluate(ra);
			if(ra.popJump()!=&j) error("Did not receive same func jumpable created");
			if(ra.guarenteedReturn)  error("Can not have return statement in constructor");
			ra.builder.CreateBr(MERGE);
			ra.guarenteedReturn = false;

			myFunction->getBasicBlockList().remove(MERGE);
			myFunction->getBasicBlockList().push_back(MERGE);
			ra.builder.SetInsertPoint(MERGE);
			ra.builder.CreateRet(thisPointer->getValue(ra));
			verifyFunction(*myFunction);
			ra.fpm->run(*myFunction);
			if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
			ret->buildFunction(ra);
		};
		ClassProto* checkTypes(RData& r) override{
			ofunction::checkTypes(r);
			ret->checkTypes(r);
			return prototype->returnType;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getFunction(myFunction,prototype);
		}
};
#endif /* OFUNCTION_HPP_ */
