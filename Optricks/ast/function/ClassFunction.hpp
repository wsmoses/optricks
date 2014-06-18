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
		Statement* const surroundingClass;
		mutable bool built;
		//TODO incorporate combined scope
		ClassFunction(PositionID id, OModule* superScope,String nam,bool st, Statement* a):
			E_FUNCTION(id, OModule(superScope),nam)
		,staticF(st),surroundingClass(a)
		,built(false)
		{
			if(name=="iterator"){
				filePos.error("Name 'iterator' is reserved for generators");
				exit(1);
			}
			module.addVariable(filePos,"this",&VOID_DATA);
		}
		void registerFunctionPrototype(RData& a) const override final{
			if(myFunction) return;
			llvm::BasicBlock* Parent = a.builder.GetInsertBlock();
			llvm::SmallVector<llvm::Type*,0> args((staticF)?(declaration.size()):(declaration.size()+1));
			std::vector<AbstractDeclaration> ad;
			auto upperClass = surroundingClass->getMyClass(a, filePos);
			if(!staticF){
				const AbstractClass* tA;
				if(!(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT))
					tA = ReferenceClass::get(upperClass);
				else tA = upperClass;
				ad.push_back(AbstractDeclaration(tA, "this"));
				args[0] = tA->type;
				ConstantData* TEMP = new ConstantData(llvm::UndefValue::get(upperClass->type),upperClass);
				module.setVariable(filePos, "this", TEMP);
			}
			for(unsigned i=0; i<declaration.size(); i++){
				const auto& b = declaration[i];
				const AbstractClass* ac = b->getClass(a, filePos);
				assert(ac);
				ad.push_back(AbstractDeclaration(ac, b->variable.pointer.name, b->value));
				args[i+(staticF?0:1)] = ac->type;
				assert(ac->type);
			}


			for (unsigned Idx = 0; Idx < declaration.size(); Idx++) {
				if(ad[Idx+(staticF?0:1)].declarationType->classType==CLASS_REF){
					auto ic = ((ReferenceClass*)ad[Idx+(staticF?0:1)].declarationType)->innerType;
					declaration[Idx]->variable.getMetadata().setObject(
						(new ConstantData(llvm::UndefValue::get(ic->type),ic))
					);
				} else{
					declaration[Idx]->variable.getMetadata().setObject(
						(new ConstantData(llvm::UndefValue::get(ad[Idx+(staticF?0:1)].declarationType->type),ad[Idx+(staticF?0:1)].declarationType))
					);
				}
			}
			const AbstractClass* returnType = (returnV)?(returnV->getMyClass(a, filePos)):(nullptr);

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
			auto FT = llvm::FunctionType::get(returnType->type, args, false);
			String nam = "_opt"+upperClass->getName()+"."+name;
			llvm::Function *F = a.CreateFunction(nam,FT, LOCAL_FUNC);
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

			llvm::BasicBlock* BB = a.CreateBlockD("entry", F);
			a.builder.SetInsertPoint(BB);

			unsigned Idx = 0;
			llvm::Function::arg_iterator AI = F->arg_begin();
			if(!staticF){
				((llvm::Value*)AI)->setName(llvm::Twine("this"));

				if(!(upperClass->layout==POINTER_LAYOUT || upperClass->layout==PRIMITIVEPOINTER_LAYOUT)){
					assert(llvm::dyn_cast<llvm::PointerType>(AI->getType()));
					Location* myLoc = getLazy("this",a, (llvm::Value*)AI,a.builder.GetInsertBlock(),nullptr);
					module.setVariable(filePos, "this", new LocationData(myLoc, upperClass));
				} else{
					module.setVariable(filePos, "this", new ConstantData(AI, upperClass));
				}
				++AI;
				//++Idx;
			}
			for (;Idx != declaration.size(); ++AI, ++Idx) {
				assert(Idx < declaration.size());
				assert(declaration[Idx]);
				((llvm::Value*)AI)->setName(llvm::Twine(myFunction->getSingleProto()->declarations[Idx+(staticF?0:1)].declarationVariable));
				if(ad[Idx+(staticF?0:1)].declarationType->classType==CLASS_REF){
					declaration[Idx]->variable.getMetadata().setObject(
						new LocationData(new StandardLocation(AI),((ReferenceClass*) ad[Idx+(staticF?0:1)].declarationType)->innerType)
					);
				} else {
					declaration[Idx]->variable.getMetadata().setObject(
						(new ConstantData(AI,ad[Idx+(staticF?0:1)].declarationType))->toLocation(a,ad[Idx+(staticF?0:1)].declarationVariable)
					);
				}
			}

			if(Parent) a.builder.SetInsertPoint( Parent );
			for(auto& d: declaration) d->registerFunctionPrototype(a);
			methodBody->registerFunctionPrototype(a);
		};
		void buildFunction(RData& a) const override final{
			if(built) return;
			built = true;
			registerFunctionPrototype(a);
			llvm::BasicBlock* Parent = a.builder.GetInsertBlock();
			a.builder.SetInsertPoint(&(myFunction->getSingleFunc()->getEntryBlock()));
			Jumpable j(name, FUNC, &module, nullptr,nullptr,myFunction->getSingleProto()->returnType);
			a.addJump(&j);
			methodBody->evaluate(a);
			if( !a.hadBreak()){
				const Data* th = module.getVariable(filePos, "this");
				for(const auto& dat: module.vars){
					if(dat!=th) decrementCount(a, filePos, dat);
				}
				if(myFunction->getSingleProto()->returnType->classType==CLASS_VOID)
					a.builder.CreateRetVoid();
				else error("Could not find return statement");
			}

			a.FinalizeFunction(myFunction->getSingleFunc());
			if(Parent) a.builder.SetInsertPoint( Parent );
			auto tmp = a.popJump();
			assert(tmp== &j);
			for(auto& d: declaration) d->buildFunction(a);
			methodBody->buildFunction(a);
		};

		void registerClasses() const override final{
			methodBody->registerClasses();
		}
};



#endif /* CLASSFUNCTION_HPP_ */
