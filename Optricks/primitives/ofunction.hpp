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
		ofunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec):oobject(id, functionClass),
				self(s),returnV(r){

			prototype = new FunctionProto((self==NULL)?"unknown":(self->getObjName()), dec, NULL);
		}

		void registerFunctionArgs(RData& r) override{
			if(prototype==NULL) error("Function prototype should not be null");
			if(returnV!=NULL) prototype->returnType = returnV->getClassProto();
			for(Declaration* d: prototype->declarations){
				d->registerFunctionArgs(r);
			}
			//if(prototype->returnType==NULL) error("Function prototype-return should not be null");
			if(self!=NULL){
				self->setFunctionProto(prototype);
				self->returnType = functionClass;
			}

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
		FunctionProto* getFunctionProto() override final{ return prototype; }
		operator String () const override{
			std::stringstream f;
			f << "function ";
			if(prototype->returnType !=NULL)  f << (prototype->returnType->name) << " ";
			f << (prototype->name) ;
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			return f.str();
		}
		ClassProto* checkTypes() override{
			for(auto& a:prototype->declarations){
				a->checkTypes();
			}
			if(self!=NULL) self->checkTypes();
			if(returnV!=NULL){
				returnV->checkTypes();
				prototype->returnType = returnV->getClassProto();
				if(prototype->returnType==NULL) error("Could not post-resolve return type "+returnV->returnType->name);
			} else if(prototype->returnType==NULL) error("Could not p-resolve return type");
			return returnType;
		}
};
//TODO
class externFunction : public ofunction{
	public:
		externFunction(PositionID id, Statement* s, Statement* r, std::vector<Declaration*> dec):
			ofunction(id, s,r,dec){
		}
		void registerFunctionArgs(RData& a){
			ofunction::registerFunctionArgs(a);
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getClassProto()->type;
				if(cl==NULL) error("Type argument "+b->classV->getObjName()+" is null");
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getClassProto();
			if(cp==NULL) error("Cannot use void class proto in extern");
			Type* r = cp->type;
			if(r==NULL) error("Type argument "+cp->name+" is null");
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, prototype->name, a.lmod);//todo check this
			if(F->getName().str()!=prototype->name) error("Cannot extern function due to name in use "+prototype->name +" was replaced with "+F->getName().str());
			self->setResolve(F);
		}
		Value* evaluate(RData& a) override{
			return self->getResolve();
		}
};
class nativeFunction : public ofunction{

};
class lambdaFunction : public ofunction{
	public:
		Statement* ret;
		lambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* r):
			ofunction(id, NULL,NULL,dec){
			ret = r;
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
		ClassProto* checkTypes() override{
			ofunction::checkTypes();
			return prototype->returnType = ret->checkTypes();
		}
		Value* evaluate(RData& ar) override{
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->getClassProto()->type;
				if(cl==NULL) error("Type argument "+b->classV->getObjName()+" is null", true);
				args.push_back(cl);
			}
			Type* r = prototype->returnType->type;
			if(r==NULL){
				error("Error null return type for class " + prototype->returnType->name);
				r = VOIDTYPE;
			}
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, "lambda", ar.lmod);//todo check this
			// Set names for all arguments.
			unsigned Idx = 0;
			for (Function::arg_iterator AI = F->arg_begin(); Idx != args.size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				prototype->declarations[Idx]->variable->pointer->resolve() = AI;
			}

			//BasicBlock *Parent = ar.builder.GetInsertBlock();
			//	ar.builder.SetInsertPoint(Parent);

			BasicBlock *Parent = ar.builder.GetInsertBlock();
			BasicBlock *BB =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "entry", F);
			ar.builder.SetInsertPoint(BB);
			Value* v = ret->evaluate(ar);
			if(r!=VOIDTYPE)
				ar.builder.CreateRet(v);
			else
				ar.builder.CreateRetVoid();
			//cout << "testing cos" << cos(3) << endl << flush;
			verifyFunction(*F);
			//cout << "verified" << endl << flush;
			ar.fpm->run(*F);

			ar.builder.SetInsertPoint( Parent );
			return F;
		}
};
class userFunction : public ofunction{
		//def int printr(int i){ int j = i+48; putchar(j); return i;}
	public:
		Statement* ret;
		userFunction(PositionID id, Statement* a, Statement* b, std::vector<Declaration*> dec, Statement* r):
			ofunction(id, a, b, dec){
			ret = r;
		}
		void registerFunctionArgs(RData& ra) override{
			ofunction::registerFunctionArgs(ra);
			ret->registerFunctionArgs(ra);
		};
		void registerFunctionDefaultArgs() override final{
			ofunction::registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			ofunction::resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes() override{
			ofunction::checkTypes();
			ret->checkTypes();
			return prototype->returnType;
		}
		Value* evaluate(RData& ra) override{
			// Set names for all arguments.
//			Function* F = dynamic_cast<Function*>(self->getResolve());
	//		if(F==NULL) error("Could not re-resolve");

			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				b->classV->checkTypes();
				Type* cl = b->classV->getClassProto()->type;
				if(cl==NULL) error("Type argument "+b->classV->getObjName()+" is null", true);
				args.push_back(cl);
			}
			ClassProto* cp = returnV->getClassProto();
			if(cp==NULL) error("Unknown return type");
			Type* r = cp->type;

			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, self->getObjName(), ra.lmod);

			self->setResolve(F);
			//BasicBlock *Parent = ar.builder.GetInsertBlock();
			//	ar.builder.SetInsertPoint(Parent);

			unsigned Idx = 0;
			for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size();
					++AI, ++Idx) {
				AI->setName(prototype->declarations[Idx]->variable->pointer->name);
				prototype->declarations[Idx]->variable->pointer->resolve() = AI;
			}
			cerr << "Made args" << endl << flush;
			BasicBlock *Parent = ra.builder.GetInsertBlock();
			BasicBlock *BB =
					//	ar.builder.GetInsertBlock();
					BasicBlock::Create(getGlobalContext(), "entry", F);
			ra.builder.SetInsertPoint(BB);
			Value* v = ret->evaluate(ra);
			//if(r!=VOIDTYPE)
			//	ar.builder.CreateRet(v);
			//else
			//	ar.builder.CreateRetVoid();
			//cout << "testing cos" << cos(3) << endl << flush;
			verifyFunction(*F);
			//cout << "verified" << endl << flush;
			ra.fpm->run(*F);

			ra.builder.SetInsertPoint( Parent );
			return F;
		}
};

#endif /* OFUNCTION_HPP_ */
