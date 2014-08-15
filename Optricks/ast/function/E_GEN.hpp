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
#include "../../language/class/GeneratorClass.hpp"
#include "../../language/location/Location.hpp"
#define E_GEN_C_
class E_GEN : public E_FUNCTION{
public:
	//friend ForEachLoop;
	mutable bool built;
	bool staticF;
	Statement* surroundingClass;
	const bool overrides;
	E_GEN(PositionID id, OModule* sur, String n,bool st,Statement* sc):
		E_FUNCTION(id,OModule(sur),n),built(false),staticF(st),surroundingClass(sc),overrides(false){
		if(!surroundingClass) assert(staticF);
		else if(!staticF)
			module.addVariable(filePos,"this",&VOID_DATA);
	}
	void reset() const override final{
		myFunction = nullptr;
		built = false;
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
	void registerFunctionPrototype(RData& a) const override{

//		filePos.compilerError("Generators not implemented");

		if(myFunction) return;

		llvm::BasicBlock* Parent = a.builder.GetInsertBlock();
		//llvm::SmallVector<llvm::Type*,0> args((staticF)?(declaration.size()):(declaration.size()+1));
		std::vector<AbstractDeclaration> ad;
		auto upperClass = (surroundingClass)?(surroundingClass->getMyClass(filePos)):nullptr;
		if(!staticF){
			assert(upperClass);
			const AbstractClass* tA;
			if(!(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT))
				tA = ReferenceClass::get(upperClass);
			else tA = upperClass;
			ad.push_back(AbstractDeclaration(tA, "this"));
		//	args[0] = tA->type;
			ConstantData* TEMP = new ConstantData(getUndef(upperClass->type),upperClass);
			module.setVariable(filePos, "this", TEMP);
		}
		std::vector<std::pair<const AbstractClass*,String>> V;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			assert(ac);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			V.push_back(std::pair<const AbstractClass*,String>(ac, b->variable.pointer.name));
		//	args[i+(staticF?0:1)] = ac->type;
			assert(ac->type);
		}


		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			if(ad[Idx+(staticF?0:1)].declarationType->classType==CLASS_REF){
				auto ic = ((ReferenceClass*)ad[Idx+(staticF?0:1)].declarationType)->innerType;
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(getUndef(ic->type),ic))
				);
			} else{
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(getUndef(ad[Idx+(staticF?0:1)].declarationType->type),ad[Idx+(staticF?0:1)].declarationType))
				);
			}
		}
		const AbstractClass* returnType = (returnV)?(returnV->getMyClass(filePos)):(nullptr);

		if(returnType==nullptr){
			std::vector<const AbstractClass*> yields;
			methodBody->collectReturns(yields,returnType);
			if(yields.size()==0) returnType = &voidClass;
			else {
				returnType = yields[0];
				for(unsigned i=0; i<yields.size(); i++)
					returnType = getMin(returnType, yields[i],filePos);
			}
		}
		assert(returnType);
		assert(returnType->type);

		String nam;
		if(upperClass) nam = upperClass->getName()+"."+name;
		else nam = name;
		auto FP = new FunctionProto(nam, ad, new GeneratorClass(this, nam, returnType, (!staticF)?upperClass:nullptr, V), false/*var arg*/);
		myFunction = new GeneratorFunction(FP, filePos);

		if(surroundingClass){
			if(staticF){
				upperClass->staticVariables.addFunction(filePos, name, nullptr)->add(myFunction, filePos);
			} else{
				if(upperClass->classType!=CLASS_USER){
					filePos.error("Cannot create class method for built-in type");
					exit(1);
				}
				((UserClass*)upperClass)->addLocalFunction(name, filePos, myFunction, overrides);
			}
		} else {
			module.surroundingScope->addFunction(filePos, name)->add(myFunction, filePos);
		}

		for(auto& d: declaration) d->registerFunctionPrototype(a);
		methodBody->registerFunctionPrototype(a);
	};
	void buildFunction(RData& a) const override final{
		registerFunctionPrototype(a);
		if(built) return;
		built = true;
		for(auto& d: declaration) d->buildFunction(a);
		methodBody->buildFunction(a);
	};
};


#endif /* E_GEN_HPP_ */
