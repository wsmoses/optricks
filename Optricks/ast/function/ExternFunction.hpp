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
private:
	E_VAR* self;
	Statement* returnV;
public:
	ExternFunction(PositionID id, std::vector<Declaration*> dec, E_VAR* s, Statement* r):
		E_FUNCTION(id,dec),self(s),returnV(r){
		assert(s);
		if(r==nullptr) id.error("Cannot have automatic return class for external function");
		assert(r);
	}
	void registerClasses() const override final{
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void buildFunction(RData& a) const override final{
		registerFunctionPrototype(a);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		//self->registerFunctionPrototype(a);
		//returnV->registerFunctionPrototype(a);
		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm:SmallVector<Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			if(ac->classType==CLASS_AUTO) error("Cannot have auto-class in function declaration");
			ad.push_back(AbstractDeclaration(ac, b->variable->pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args[i] = cl;
		}
		const AbstractClass* returnType = returnV->getSelfClass(filePos);
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, args, false);
		String nam = self->getShortName();
		llvm::Function *F = a.CreateFunctionD(nam,FT, EXTERN_FUNC);
		/*if(nam=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
		else if(nam=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
		else if(nam=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
		else if(nam=="printby") a.exec->addGlobalMapping(F, (void*)(&printby));
		else if(nam=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
		else if(nam=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
		else*/
		if(F->getName().str()!=nam){
			error("Cannot extern function due to name in use "+nam+" was replaced with "+F->getName().str());
		}
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);
		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
	}
};



#endif /* EXTERNFUNCTION_HPP_ */
