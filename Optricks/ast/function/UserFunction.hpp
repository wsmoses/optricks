/*
 * UserFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef USERFUNCTION_HPP_
#define USERFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
class UserFunction : public E_FUNCTION{
private:
	E_VAR* self;
	Statement* returnV;
	Statement* body;
	SingleFunction* myFunction;
	bool built;
public:
	UserFunction(PositionID id, std::vector<Declaration*> dec, E_VAR* s, Statement* r, Statement* b):
		E_FUNCTION(id,dec),self(s),returnV(r),body(b),myFunction(nullptr),built(false){
		assert(s);
		assert(r);
	}
	void registerClasses() const override final{
		body->registerClasses();
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void write(ostream& f, String b) const override{
		f << "def ";
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
					DATA::getConstant(AI,myFunction->getSingleProto()->declarations[Idx].declarationType).toLocation(ra));
		}
		body->evaluate(ra);
		if(! ra.hadBreak()){
			if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
				ra.builder.CreateRetVoid();
			else error("Could not find return statement");
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
		const AbstractClass* returnType = returnV->getSelfClass(filePos);

		if(returnType->classType==CLASS_AUTO){
			std::vector<const AbstractClass*> yields;
			body->collectReturns(yields,returnType);
			if(yields.size()==0) returnType = voidClass;
			else {
				returnType = getMin(yields,filePos);
				if(returnType->classType==CLASS_AUTO)
					filePos.compilerError("Cannot deduce return type of function "+self->getFullName());
			}
		}
		assert(returnType);
		llvm::Type* r = returnType->type;
		FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
		String nam = "!"+((self)?(self->getShortName()):("anon"));
		llvm::Function *F = a.CreateFunctionD(nam,FT, LOCAL_FUNC);
		myFunction = new CompiledFunction(new FunctionProto(self->getFullName(), ad, returnType), F);
		self->getMetadata().addFunction(myFunction);
		if(Parent!=NULL) a.builder.SetInsertPoint( Parent );
		body->registerFunctionPrototype(a);
	}
};




#endif /* USERFUNCTION_HPP_ */
