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
		Resolvable* self;
		Resolvable* returnV;
		FunctionProto* prototype;
		ofunction(PositionID id, Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):oobject(id, functionClass),
				self(s),returnV(r){

			prototype = new FunctionProto((self==NULL)?"unknown":(self->name), dec, NULL);
		}

		void registerFunctionArgs(RData& r) override final{
			if(self!=NULL){
				if(prototype==NULL) error("Function prototype should not be null");
				self->resolveFunction() = prototype;
				self->resolveReturnClass() = functionClass;
			}

		};
		void registerFunctionDefaultArgs() override final{

		};
		void resolvePointers() override final{

		};
		FunctionProto* getFunctionProto() override final{ return prototype; }
		operator String () const override{
			std::stringstream f;
			f << "function ";
			if(returnV !=NULL)  f << (returnV->name) << " ";
			if(self !=NULL ) f << (self->name);
			f << "(";
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", ";
				a->write(f,"");
			}
			f << ")";
			return f.str();
		}
		ClassProto* checkTypes() override{
			for(auto& a:prototype->declarations){
				a->checkTypes();
			}
			prototype->returnType = returnV->resolveSelfClass();
			if(prototype->returnType==NULL) todo("Could not post-resolve return type "+returnV->name);
			return returnType;
		}
};
//TODO
class externFunction : public ofunction{
	public:
		externFunction(PositionID id, Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):
			ofunction(id, s,r,dec){
		}
		Value* evaluate(RData& a) override{
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->pointer->resolveSelfClass()->type;
				if(cl==NULL) todo("Type argument "+b->classV->pointer->name+" is null");
				args.push_back(cl);
			}
			Type* r = returnV->resolveSelfClass()->type;
			if(r==NULL) todo("Type argument "+returnV->name+" is null");
			FunctionType *FT = FunctionType::get(r, args, false);
			Function *F = Function::Create(FT, Function::ExternalLinkage, self->name, a.lmod);//todo check this
			self->resolve() = F;
			return F;
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
		ClassProto* checkTypes() override{
			for(auto& a:prototype->declarations){
				a->checkTypes();
			}
			return prototype->returnType = ret->checkTypes();
		}
		Value* evaluate(RData& ar) override{
			std::vector<Type*> args;
			for(auto & b: prototype->declarations){
				Type* cl = b->classV->pointer->resolveSelfClass()->type;
				if(cl==NULL) error("Type argument "+b->classV->pointer->name+" is null", true);
				args.push_back(cl);
			}
			Type* r = prototype->returnType->type;
			if(r==NULL){
				todo("Error null return type for class " + prototype->returnType->name);
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

};

#endif /* OFUNCTION_HPP_ */
