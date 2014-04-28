/*
 * ConstructorFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef CONSTRUCTORFUNCTION_HPP_
#define CONSTRUCTORFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../language/location/Location.hpp"

class ConstructorFunction : public E_FUNCTION{
private:
	Statement* myClass;
	bool built;
public:
	//TODO combine scope
	ConstructorFunction(PositionID id, OModule* superScope,Statement* mc):
//std::vector<Declaration*> dec, const Resolvable& r, Statement* s, Statement* b):
		E_FUNCTION(id,OModule(superScope),"#constructor"),myClass(mc),built(false){
		module.addVariable(filePos,"this",&VOID_DATA);
	}
	void registerClasses() const override final{
		methodBody->registerClasses();
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void buildFunction(RData& ra) const override final{
		if(built) return;
		registerFunctionPrototype(ra);
		assert(returnV==nullptr);

		BasicBlock *Parent = ra.builder.GetInsertBlock();
		llvm::Function* F = myFunction->getSingleFunc();
		ra.builder.SetInsertPoint(& (F->getEntryBlock()));

		auto uc = myFunction->getSingleProto()->returnType;
		assert(uc->classType==CLASS_USER);
		auto UC = (const UserClass*)uc;
		if(uc->layout==PRIMITIVE_LAYOUT)
			module.setVariable(filePos, "this",(new ConstantData( UC->generateData(ra, filePos), uc))->toLocation(ra));
		else
			module.setVariable(filePos, "this",new ConstantData( UC->generateData(ra, filePos), uc));

		auto tmp = ra.functionReturn;
		ra.functionReturn = nullptr;
		methodBody->evaluate(ra);
		if( ra.hadBreak()){
			error("Cannot use return in constructor");
		}
		const Data* th = module.getVariable(filePos, "this");
		Value* V = th->getValue(ra,filePos);

		for(const auto& dat: module.vars){
			if(dat!=th) decrementCount(ra, filePos, dat);
		}
		ra.builder.CreateRet(V);
		ra.FinalizeFunction(F);
		if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		assert(ra.functionReturn == nullptr);
		ra.functionReturn = tmp;
		methodBody->buildFunction(ra);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		assert(returnV==nullptr);
		//self->registerFunctionPrototype(a);
		//returnV->registerFunctionPrototype(a);
		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<Type*,1> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			Type* cl = ac->type;
			if(cl==NULL) error("Type argument "+ac->getName()+" is null");
			args[i] = cl;
		}
		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			if(ad[Idx].declarationType->classType==CLASS_REF){
				auto ic = ((ReferenceClass*)ad[Idx].declarationType)->innerType;
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(UndefValue::get(ic->type),ic))
				);
			} else{
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(UndefValue::get(ad[Idx].declarationType->type),ad[Idx].declarationType))
				);
			}
		}
		const AbstractClass* returnType = myClass->getSelfClass(filePos);
		assert(returnType);
		if(returnType->classType!=CLASS_USER) filePos.error("Cannot make constructor for built-in types");
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, args, false);
		String nam = "!"+(returnType->getName());
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto(returnType->getName(), ad, returnType), F);
		((const UserClass*)returnType)->constructors.add((SingleFunction*)myFunction, filePos);
		BasicBlock *BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);
		unsigned Idx = 0;
		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((Value*)AI)->setName(Twine(myFunction->getSingleProto()->declarations[Idx].declarationVariable));
			if(ad[Idx].declarationType->classType==CLASS_REF){
				declaration[Idx]->variable.getMetadata().setObject(
					new LocationData(new StandardLocation(AI),((ReferenceClass*) ad[Idx].declarationType)->innerType)
				);
			} else {
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(AI,ad[Idx].declarationType))->toLocation(a)
				);
			}
		}

		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
		methodBody->registerFunctionPrototype(a);
	}
};




#endif /* CONSTRUCTORFUNCTION_HPP_ */
