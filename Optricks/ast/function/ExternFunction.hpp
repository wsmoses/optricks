/*
 * ExternFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef EXTERNFUNCTION_HPP_
#define EXTERNFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../language/basic_functions.h"
class ExternFunction : public E_FUNCTION{
public:
	ExternFunction(PositionID id, OModule* o,String n):
		E_FUNCTION(id,OModule(o),n){
		returnV=nullptr;
	}
	void registerClasses() const override final{
		//returnV->registerClasses();
	}
	void buildFunction(RData& a) const override final{
		registerFunctionPrototype(a);
		assert(methodBody==nullptr);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		assert(methodBody==nullptr);
		//returnV->registerFunctionPrototype(a);
		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args[i] = cl;
		}
		assert(returnV);
		const AbstractClass* returnType = returnV->getSelfClass(filePos);
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, args, false);
		llvm::Function *F = a.getExtern(name, FT);//a.CreateFunctionD(nam,FT, EXTERN_FUNC);
		/*if(nam=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
		else if(nam=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
		else if(nam=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
		else if(nam=="printby") a.exec->addGlobalMapping(F, (void*)(&printby));
		else if(nam=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
		else if(nam=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
		else*/
		if(F->getName().str()!=name){
			filePos.error("Cannot extern function due to name in use "+name+" was replaced with "+F->getName().str());
		}
		//todo have full name
		myFunction = new CompiledFunction(new FunctionProto(name, ad, returnType), F);
		module.surroundingScope->addFunction(filePos, name)->add(myFunction, filePos);
		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
	}
};



#endif /* EXTERNFUNCTION_HPP_ */
