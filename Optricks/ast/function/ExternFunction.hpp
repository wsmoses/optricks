/*
 * ExternFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef EXTERNFUNCTION_HPP_
#define EXTERNFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
class ExternFunction : public E_FUNCTION{
private:
	E_VAR* self;
	Statement* returnV;
	CompiledFunction* myFunction;
public:
	ExternFunction(PositionID id, std::vector<Declaration*> dec, E_VAR* s, Statement* r):
		E_FUNCTION(id,dec),self(s),returnV(r),myFunction(nullptr){
		assert(s);
		assert(r);
	}
	void registerClasses() const override final{
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void write(ostream& f, String b) const override{
		f << "ext ";
		f << returnV->getSelfClass(filePos)->getName() << " ";
		f << self->getFullName() ;
		f << "(" ;
		bool first = true;
		for(auto &a: declaration){
			if(first) first = false;
			else f << ", " ;
			a->write(f,"");
		}
		f << ")";
	}
	void buildFunction(RData& a){
		registerFunctionPrototype(a);
	}
	void registerFunctionPrototype(RData& a){
		if(myFunction) return;
		//self->registerFunctionPrototype(a);
		//returnV->registerFunctionPrototype(a);
		BasicBlock *Parent = a.builder.GetInsertBlock();
		std::vector<Type*> args;
		std::vector<AbstractDeclaration> ad;
		for(auto & b: declaration){
			const AbstractClass* ac = b->getClass(filePos);
			if(ac->classType==CLASS_AUTO) error("Cannot have auto-class in function declaration");
			ad.push_back(AbstractDeclaration(ac, b->variable->pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args.push_back(cl);
		}
		const AbstractClass* returnType = returnV->getSelfClass(filePos);
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
		String nam = self->getShortName();
		llvm::Function *F = a.CreateFunctionD(nam,FT, EXTERN_FUNC);
		/*if(nam=="printi") a.exec->addGlobalMapping(F, (void*)(&printi));
		else if(nam=="printd") a.exec->addGlobalMapping(F, (void*)(&printd));
		else if(nam=="printb") a.exec->addGlobalMapping(F, (void*)(&printb));
		else if(nam=="printby") a.exec->addGlobalMapping(F, (void*)(&printby));
		else if(nam=="prints") a.exec->addGlobalMapping(F, (void*)(&prints));
		else if(nam=="printc") a.exec->addGlobalMapping(F, (void*)(&printc));
		else */
		if(F->getName().str()!=nam){
			error("Cannot extern function due to name in use "+nam+" was replaced with "+F->getName().str());
		}
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);
		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
	}
};



#endif /* EXTERNFUNCTION_HPP_ */
