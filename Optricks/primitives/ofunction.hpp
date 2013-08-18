/*
 * ofunction.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OFUNCTION_HPP_
#define OFUNCTION_HPP_

#include "oobject.hpp"
#include "../constructs/Declaration.hpp"
class ofunction:public oobject{
	public:
		Resolvable* self;
		Resolvable* returnV;
		std::vector<Declaration*> declarations;
		ofunction(Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):oobject(functionClass),
			self(s),returnV(r),declarations(dec){
			self->resolveMeta() = this;
		}

		operator String () const override{
			std::stringstream f;
			f << "function " << returnV->name << " " << self->name << "(";
			bool first = true;
			for(auto &a: declarations){
				if(first) first = false;
				else f << ", ";
				a->write(f,"");
			}
			f << ")";
			return f.str();
		}
		void checkTypes(){
			for(auto& a:declarations){
				a->checkTypes();
			}
		}
};
//TODO
class externFunction : public ofunction{
	public:
		externFunction(Resolvable* s, Resolvable* r, std::vector<Declaration*> dec):ofunction(s,r,dec){
		}
	Value* evaluate(RData& a,LLVMContext& context) override{
		//TODO implement
		std::vector<Type*> args;
		for(auto & b: declarations){
			Type* cl = b->classV->pointer->resolveType();
			if(cl==NULL) todo("Type argument "+b->classV->pointer->name+" is null");
			args.push_back(cl);
		}
		Type* r = returnV->resolveType();
		if(r==NULL) todo("Type argument "+returnV->name+" is null");
		  FunctionType *FT = FunctionType::get(r, args, false);
		  Function *F = Function::Create(FT, Function::ExternalLinkage, self->name, NULL);//todo check this
		  self->resolve() = F;
		  // Set names for all arguments.
		 /* unsigned Idx = 0;
		  for (Function::arg_iterator AI = F->arg_begin(); Idx != args.size();
		       ++AI, ++Idx) {
		    AI->setName(args[Idx]);

		    // Add arguments to variable symbol table.
		    NamedValues[args[Idx]] = AI;
		  }*/
		  return F;
//		todo("Extern not implemented");
	}
};
class nativeFunction : public ofunction{

};
class lambdaFunction : public ofunction{

};
class userFunction : public ofunction{

};

#endif /* OFUNCTION_HPP_ */
