/*
 * ConstructorFunction.hpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Billy
 */

#ifndef CONSTRUCTORFUNCTION_HPP_
#define CONSTRUCTORFUNCTION_HPP_
#include "./E_FUNCTION.hpp"

class ConstructorFunction : public E_FUNCTION{
private:
	E_VAR* self;
	Statement* body;
	SingleFunction* myFunction;
	Resolvable _this;
	bool built;
public:
	ConstructorFunction(PositionID id, std::vector<Declaration*> dec, const Resolvable& r, E_VAR* s, Statement* b):
		E_FUNCTION(id,dec),self(s),body(b),myFunction(nullptr),_this(r),built(false){
		assert(s);
	}
	void registerClasses() const override final{
		body->registerClasses();
		//self->registerClasses();
		//returnV->registerClasses();
	}
	void write(ostream& f, String b) const override{
		f << "def ";
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
					getLocationData(ra, AI,myFunction->getSingleProto()->declarations[Idx].declarationType));
		}
		_this.setObject(myFunction->getSingleProto()->returnType->generateData());
		body->evaluate(ra);
		if( ra.hadBreak()){
			error("Cannot use return in constructor");
		}
		ra.builder.CreateRet(_this.getValue(ra));
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
		const AbstractClass* returnType = self->getSelfClass(filePos);

		if(returnType->classType==CLASS_AUTO){
			filePos.compilerError("Cannot create constructor for auto class");
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




#endif /* CONSTRUCTORFUNCTION_HPP_ */
