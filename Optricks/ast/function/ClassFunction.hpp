/*
 * ClassFunction.hpp
 *
 *  Created on: Feb 11, 2014
 *      Author: Billy
 */

#ifndef CLASSFUNCTION_HPP_
#define CLASSFUNCTION_HPP_
#include "./E_FUNCTION.hpp"
#include "../../language/class/builtin/ReferenceClass.hpp"
#include "../../language/class/ClassLib.hpp"
#include "../../language/location/Location.hpp"
class ClassFunction : public E_FUNCTION{
	public:
		const bool staticF;
		const Resolvable thisPointer;
		const String name;
		Statement* const surroundingClass;
		Statement* const returnV;
		Statement* const body;
		mutable bool built;
		ClassFunction(PositionID id, std::vector<Declaration*> dec, bool st, Resolvable tPointer,String nam, Statement* a, Statement* b, Statement* r):
			E_FUNCTION(id, dec),staticF(st),thisPointer(tPointer),name(nam),surroundingClass(a),returnV(b),body(r),built(false){
		}
		void registerFunctionPrototype(RData& a) const override final{

			BasicBlock *Parent = a.builder.GetInsertBlock();
			std::vector<Type*> args;
			std::vector<AbstractDeclaration> ad;
			AbstractClass* upperClass = surroundingClass->getSelfClass(filePos);
			if(!staticF){
				const AbstractClass* tA;
				if(!(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT))
					tA = ReferenceClass::get(upperClass);
				else tA = upperClass;
				ad.push_back(AbstractDeclaration(tA, "this"));
				args.push_back(tA->type);
			}
			for(auto & b: declaration){
				const AbstractClass* ac = b->getClass(filePos);
				if(ac->classType==CLASS_AUTO) error("Cannot have auto-class in function declaration");
				ad.push_back(AbstractDeclaration(ac, b->variable->pointer.name, b->value));
				args.push_back(ac->type);
			}
			const AbstractClass* returnType = (returnType)?(returnV->getSelfClass(filePos)):(nullptr);

			if(returnType==nullptr){
				std::vector<const AbstractClass*> yields;
				body->collectReturns(yields,returnType);
				if(yields.size()==0) returnType = voidClass;
				else {
					for(auto& a: yields) assert(a->classType!=CLASS_AUTO);
					returnType = getMin(yields,filePos);
					if(returnType->classType==CLASS_AUTO)
						filePos.compilerError("Cannot deduce return type of function "+upperClass->getName()+"."+name);
				}
			}
			assert(returnType);
			llvm::Type* r = returnType->type;
			FunctionType *FT = FunctionType::get(r, ArrayRef<Type*>(args), false);
			String nam = "!"+upperClass->getName()+"."+name;
			llvm::Function *F = a.CreateFunctionD(nam,FT, LOCAL_FUNC);
			myFunction = new CompiledFunction(new FunctionProto(upperClass->getName()+"."+name, ad, returnType), F);

			if(staticF){
				upperClass->staticVariables.addFunction(filePos, name, nullptr)->add(myFunction, filePos);
			} else{
				if(upperClass->classType!=CLASS_USER){
					filePos.error("Cannot create class method for built-in type");
					exit(1);
				}
				((UserClass*)upperClass)->addLocalFunction(name)->add(myFunction, filePos);
			}

			BasicBlock *BB = a.CreateBlockD("entry", F);
			a.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			Function::arg_iterator AI = F->arg_begin();
			if(!staticF){
				((Value*)AI)->setName(Twine("this"));

				if(!(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT)){
					assert(dyn_cast<PointerType>(AI->getType()));
					Type* t = ((PointerType*)(((Value*)AI)->getType()))->getElementType();
					Location* myLoc;
					if(t->isAggregateType() || t->isArrayTy() || t->isVectorTy() || t->isStructTy()){
						myLoc = new StandardLocation(AI);
					} else {
						myLoc = new LazyLocation(a,AI,a.builder.GetInsertBlock(),NULL);
					}
					thisPointer.setObject(new LocationData(myLoc, upperClass));
				} else thisPointer.setObject(new ConstantData(AI, upperClass));
				++AI;
				++Idx;
			}
			for (;Idx != F->arg_size(); ++AI, ++Idx) {
				((Value*)AI)->setName(Twine(myFunction->getSingleProto()->declarations[Idx].declarationVariable));
				declaration[Idx]->variable->getMetadata().setObject(
					(new ConstantData(AI,myFunction->getSingleProto()->declarations[Idx].declarationType))->toLocation(a)
				);
			}

			if(Parent) a.builder.SetInsertPoint( Parent );
			body->registerFunctionPrototype(a);
		};
		void buildFunction(RData& a) const override final{
			if(built) return;
			built = true;
			registerFunctionPrototype(a);
			BasicBlock *Parent = a.builder.GetInsertBlock();
			a.builder.SetInsertPoint(&(myFunction->getSingleFunc()->getEntryBlock()));
			body->evaluate(a);
			if( !a.hadBreak()){
				if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
					a.builder.CreateRetVoid();
				else error("Could not find return statement");
			}

			a.FinalizeFunction(myFunction->getSingleFunc());
			if(Parent) a.builder.SetInsertPoint( Parent );
			body->buildFunction(a);
		};

		void registerClasses() const override final{
			body->registerClasses();
		}
};



#endif /* CLASSFUNCTION_HPP_ */
