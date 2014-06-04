/*
 * LambdaFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef LAMBDAFUNCTION_HPP_
#define LAMBDAFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../operators/Deconstructor.hpp"
class LambdaFunction : public E_FUNCTION{
private:
	bool built;
public:
	LambdaFunction(PositionID id, OModule* superMod):
		E_FUNCTION(id,OModule(superMod),""),built(false){
	}
	void registerClasses() const override final{
		methodBody->registerClasses();
	}
	void buildFunction(RData& ra) const override final{
		if(built) return;
		registerFunctionPrototype(ra);

		llvm::BasicBlock* Parent = ra.builder.GetInsertBlock();
		llvm::Function* F = myFunction->getSingleFunc();
		ra.builder.SetInsertPoint(& (F->getEntryBlock()));
		Jumpable j(name, FUNC, &module, nullptr,nullptr,nullptr);
		ra.addJump(&j);

		const Data* ret = methodBody->evaluate(ra);
		assert(!ra.hadBreak());
		for(const auto& a: module.vars){
			decrementCount(ra, filePos, a);
		}
		if(ret->type==R_VOID)
			ra.builder.CreateRetVoid();
		else{
			llvm::Value* V = ret->getValue(ra, filePos);
			ra.builder.CreateRet(V);
		}
		ra.FinalizeFunction(F);
		if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		auto tmp = ra.popJump();
		assert(tmp== &j);

		methodBody->buildFunction(ra);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		//self->registerFunctionPrototype(a);
		//returnV->registerFunctionPrototype(a);
		llvm::BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<llvm::Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(a, filePos);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			assert(ac);
			llvm::Type* cl = ac->type;
			assert(cl);
			args[i] = cl;
		}
		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			if(ad[Idx].declarationType->classType==CLASS_REF){
				auto ic = ((ReferenceClass*)ad[Idx].declarationType)->innerType;
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(llvm::UndefValue::get(ic->type),ic))
				);
			} else{
				declaration[Idx]->variable.getMetadata().setObject(
					(new ConstantData(llvm::UndefValue::get(ad[Idx].declarationType->type),ad[Idx].declarationType))
				);
			}
		}
		const AbstractClass* returnType = methodBody->getReturnType();
		assert(returnType);
		llvm::Type* r = returnType->type;
		assert(r);
		auto FT = llvm::FunctionType::get(r, args, false);
		String nam = "!lambda";
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto("lambda", ad, returnType), F);

		llvm::BasicBlock* BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((llvm::Value*)AI)->setName(llvm::Twine(ad[Idx].declarationVariable));
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

		if(Parent) a.builder.SetInsertPoint( Parent );
		methodBody->registerFunctionPrototype(a);
	}
};




#endif /* LAMBDAFUNCTION_HPP_ */
