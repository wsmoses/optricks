/*
 * E_GEN.hpp
 *
 *  Created on: Oct 18, 2013
 *      Author: wmoses
 */

#ifndef E_GEN_HPP_
#define E_GEN_HPP_

#include "../../language/statement/Statement.hpp"
#include "./E_FUNCTION.hpp"
#include "../../language/class/ClassLib.hpp"
#include "../../language/location/Location.hpp"
#define E_GEN_C_
class E_GEN : public E_FUNCTION{
public:
	mutable bool built;
	E_GEN(PositionID id, OModule* sur, String n):
		E_FUNCTION(id,OModule(sur),n),built(false){
	}
	void registerClasses() const override final{
		methodBody->registerClasses();
	}
/*
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		BasicBlock *Parent = a.builder.GetInsertBlock();
		std::vector<AbstractDeclaration> ad;
		for(auto & b: declaration){
			const AbstractClass* ac = b->getClass(filePos);
			if(ac->classType==CLASS_AUTO) error("Cannot have auto-class in generator declaration");
			ad.push_back(AbstractDeclaration(ac, b->variable->pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
		}
		const AbstractClass* returnType = (returnType)?(returnV->getSelfClass(filePos)):(nullptr);

		if(returnType==nullptr){
			std::vector<const AbstractClass*> yields;
			methodBody->collectReturns(yields,returnType);
			if(yields.size()==0) returnType = voidClass;
			else {
				returnType = getMin(yields,filePos);
				if(returnType->classType==CLASS_AUTO)
					filePos.compilerError("Cannot deduce return type of generator "+self->getFullName());
			}
		}
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, args, false);
		String nam = "!"+((self)?(self->getShortName()):("anon"));
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);

		BasicBlock *BB = a.CreateBlock1("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			AI->setName(myFunction->getSingleProto()->declarations[Idx].declarationVariable);
			declaration[Idx]->variable->getMetadata().setObject(
				(new ConstantData(AI,myFunction->getSingleProto()->declarations[Idx].declarationType))->toLocation(a)
			);
		}

		if(Parent) a.builder.SetInsertPoint( Parent );
		methodBody->registerFunctionPrototype(a);
	}*/
	const Token getToken() const{
		return T_GEN;
	}
	void registerFunctionPrototype(RData& ra) const override{

		filePos.compilerError("Generators not implemented");
		/*
		if(registereD) return;
		registereD = true;
		for(auto& a:declaration) a->registerFunctionPrototype(ra);
		methodBody->registerFunctionPrototype(ra);
		self->pointer.addFunction();
		std::vector<const AbstractClass*> cp;
		const AbstractClass* ret = (returnV)?(returnV->getSelfClass(filePos)):(nullptr);
		methodBody->collectReturns(cp,myFunction->getSingleProto()->returnType);
		if(!ret){
			if(cp.size()==0){
				filePos.error("Cannot have auto-returning generator with no yield statements");
				ret=voidClass;
			}
			else{
				const AbstractClass* c = getMin(cp, filePos);
				assert(c);
				if(c->classType==CLASS_VOID) filePos.error("Cannot have void yields");
				ret = c;
			}
		}
		//TODO use ret to build generator
		*/
	};
	void buildFunction(RData& r) const override final{
		filePos.compilerError("Generators not implemented");
		/*
		registerFunctionPrototype(r);
		if(builtF) return;
		builtF = true;
		if(returnClass!=NULL) returnClass->buildFunction(r);
		for(auto& a:declaration) a->buildFunction(r);
		self->buildFunction(r);
		methodBody->buildFunction(r);*/
	};
};


#endif /* E_GEN_HPP_ */
