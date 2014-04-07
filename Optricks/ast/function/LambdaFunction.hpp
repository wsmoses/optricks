/*
 * LambdaFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef LAMBDAFUNCTION_HPP_
#define LAMBDAFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
class LambdaFunction : public E_FUNCTION{
private:
	Statement* body;
	bool built;
public:
	LambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* b):
		E_FUNCTION(id,dec),body(b),built(false){
	}
	void registerClasses() const override final{
		body->registerClasses();
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void buildFunction(RData& ra) const override final{
		if(built) return;
		registerFunctionPrototype(ra);

		BasicBlock *Parent = ra.builder.GetInsertBlock();
		llvm::Function* F = myFunction->getSingleFunc();
		ra.builder.SetInsertPoint(& (F->getEntryBlock()));
		auto tmp = ra.functionReturn;
		ra.functionReturn = nullptr;

		const Data* ret = body->evaluate(ra);
		if(! ra.hadBreak()){
			if(ret->type==R_VOID)
				ra.builder.CreateRetVoid();
			else{
				Value* V = ret->getValue(ra, filePos);
				ra.builder.CreateRet(V);
			}
		}
		ra.FinalizeFunction(F);
		if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		assert(ra.functionReturn == nullptr);
		ra.functionReturn = tmp;
		body->buildFunction(ra);
	}
	void registerFunctionPrototype(RData& a) const override final{
		if(myFunction) return;
		//self->registerFunctionPrototype(a);
		//returnV->registerFunctionPrototype(a);
		BasicBlock *Parent = a.builder.GetInsertBlock();
		llvm::SmallVector<Type*,0> args(declaration.size());
		std::vector<AbstractDeclaration> ad;
		for(unsigned i=0; i<declaration.size(); i++){
			const auto& b = declaration[i];
			const AbstractClass* ac = b->getClass(filePos);
			ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
			assert(ac);
			Type* cl = ac->type;
			assert(cl);
			args[i] = cl;
		}
		for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
			declaration[Idx]->variable.getMetadata().setObject(
				(new ConstantData(UndefValue::get(ad[Idx].declarationType->type),ad[Idx].declarationType))
			);
		}
		const AbstractClass* returnType = body->getReturnType();
		assert(returnType);
		llvm::Type* r = returnType->type;
		assert(r);
		FunctionType *FT = FunctionType::get(r, args, false);
		String nam = "!lambda";
		llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto("lambda", ad, returnType), F);

		BasicBlock *BB = a.CreateBlockD("entry", F);
		a.builder.SetInsertPoint(BB);

		unsigned Idx = 0;
		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size(); ++AI, ++Idx) {
			((Value*)AI)->setName(Twine(ad[Idx].declarationVariable));
			declaration[Idx]->variable.getMetadata().setObject(
			(new ConstantData((Value*)AI,ad[Idx].declarationType))->toLocation(a)
			);
		}

		if(Parent) a.builder.SetInsertPoint( Parent );
		body->registerFunctionPrototype(a);
	}
};




#endif /* LAMBDAFUNCTION_HPP_ */
