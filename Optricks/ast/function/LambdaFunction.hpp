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
	CompiledFunction* myFunction;
	bool built;
public:
	LambdaFunction(PositionID id, std::vector<Declaration*> dec, Statement* b):
		E_FUNCTION(id,dec),body(b),myFunction(nullptr),built(false){
	}
	void registerClasses() const override final{
		body->registerClasses();
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void write(ostream& f, String b) const override{
		f << "lambda ";
		bool first = true;
		for(auto &a: declaration){
			if(first) first = false;
			else f << ", " ;
			a->write(f,"");
		}
		f << ": ";
		body->write(f, b+"  ");
	}
	void buildFunction(RData& ra){
		if(built) return;
		registerFunctionPrototype(ra);

		BasicBlock *Parent = ra.builder.GetInsertBlock();
		llvm::Function* F = myFunction->getSingleFunc();
		BasicBlock *BB = ra.CreateBlock1("entry", F);
		ra.builder.SetInsertPoint(BB);

		unsigned Idx = 0;

		for (Function::arg_iterator AI = F->arg_begin(); Idx != F->arg_size();
				++AI, ++Idx) {
			AI->setName(myFunction->getSingleProto()->declarations[Idx].declarationVariable);
			declaration[Idx]->variable->getMetadata().setObject(
					(new ConstantData(AI,myFunction->proto->declarations[Idx].declarationType))
					->toLocation(ra));
		}
		const Data* ret = body->evaluate(ra);
		if(! ra.hadBreak()){
			if(ret->type==R_VOID)
				ra.builder.CreateRetVoid();
			else{
				ra.builder.CreateRet(ret->getValue(ra, filePos));
			}
		}
		ra.FinalizeFunction(F);
		if(Parent!=NULL) ra.builder.SetInsertPoint( Parent );
		body->buildFunction(ra);
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
		const AbstractClass* returnType = body->getReturnType();

		if(returnType->classType==CLASS_AUTO){
			filePos.compilerError("Cannot deduce return type of lambda function");
		}
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
		String nam = "!lambda";
		llvm::Function *F = a.CreateFunctionD(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto("lambda", ad, returnType), F);
		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
		body->registerFunctionPrototype(a);
	}
};




#endif /* LAMBDAFUNCTION_HPP_ */
