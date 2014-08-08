/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../language/statement/Statement.hpp"
#include "./E_VAR.hpp"
#include "../language/location/Location.hpp"
#include "../language/class/builtin/ReferenceClass.hpp"
#include "../language/class/builtin/FunctionClass.hpp"
#include "../language/class/builtin/LazyClass.hpp"
#include "../language/data/ReferenceData.hpp"
#include "../operators/Deconstructor.hpp"

#define DECLR_P_
class Declaration: public ErrorStatement{
private:
	friend DeclarationData;
	Statement* classV;
	mutable const AbstractClass* returnType;
	mutable unsigned isReference;
	mutable const LocationData* finished;
public:
	const AbstractClass* getMyClass(PositionID id)const override{
		id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
	}
	const AbstractClass* getClass(PositionID id)const{
		if(classV){
			return classV->getMyClass(id);
		}
		else{
			id.error("Cannot use auto declaration");
			exit(1);
		}
	}
	E_VAR variable;
	Statement* value;
	bool global;
	Declaration(PositionID id, Statement* v, const E_VAR& loc, bool glob, Statement* e) :
		ErrorStatement(id),
	classV(v),returnType(nullptr),isReference(2),finished(nullptr),variable(loc),value(e),global(glob){
	}
	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
	}
	bool hasValue() const {
		return value!=NULL && value->getToken()!=T_VOID;
	}
	const Token getToken() const final override{
		return T_DECLARATION;
	}
	const AbstractClass* getReturnType() const final override{
		if(returnType) return returnType;
		if(!classV){
			if(value){
				returnType = value->getReturnType();
				if(returnType->classType==CLASS_VOID){
					filePos.error("Cannot have void declaration");
					exit(1);
				}
				if(returnType->classType==CLASS_REF){
					returnType = ((const ReferenceClass*)returnType)->innerType;
					isReference = 1;
				} else isReference = 0;
				return returnType;
			} else {
				filePos.error("Cannot have auto declaration without default value");
				//TODO implement searching for correct variable type
				exit(1);
			}
		}
		returnType = classV->getMyClass(filePos);
		if(returnType->classType==CLASS_REF){
			returnType = ((ReferenceClass*)returnType)->innerType;
			isReference = 1;
		} else isReference = 0;
		assert(returnType);
		if(returnType->classType==CLASS_VOID){
			filePos.error("Cannot have void declaration");
			exit(1);
		}
		return returnType;
	}

	const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
		assert(isClassMethod==false);
		getReturnType();
		if(returnType->classType==CLASS_FUNC){
			return ((FunctionClass*)returnType)->returnType;
		}  else if(returnType->classType==CLASS_LAZY){
			return ((LazyClass*)returnType)->innerType;
		} else if(returnType->classType==CLASS_CLASS){
			return returnType;
		}	else {
			id.error("Class '"+returnType->getName()+"' cannot be used as function");
			exit(1);
		}
	}
	void registerClasses() const override final{
		if(classV) classV->registerClasses();
		variable.registerClasses();
		if(value) value->registerClasses();
	}
	void registerFunctionPrototype(RData& r) const override final{
		if(classV) classV->registerFunctionPrototype(r);
		variable.registerFunctionPrototype(r);
		if(value) value->registerFunctionPrototype(r);
	};
	void buildFunction(RData& r) const override final{
		if(classV) classV->buildFunction(r);
		variable.buildFunction(r);
		if(value) value->buildFunction(r);
	};
	void reset() const override final{
		finished = nullptr;
	}
	Location* fastEvaluate(){
		if(finished) return finished->value;
		getReturnType();
		assert(returnType);
		if(returnType->layout==LITERAL_LAYOUT){
			filePos.error("Cannot get value of literal, please wrap with finite class like int/string");
			exit(1);
		}
		assert(returnType->type);
		if(isReference == 1){
			filePos.error("Cannot find references early");
		}
		Location* loc;
		assert(global);
		llvm::Constant* VAL;
		if(returnType->layout==POINTER_LAYOUT){
			assert(returnType->type);
			assert(llvm::dyn_cast<llvm::PointerType>(returnType->type));
			VAL = llvm::ConstantPointerNull::get((llvm::PointerType*)returnType->type);
		}
		else
			VAL = rdata.getGlobal(returnType->type,false);
		llvm::GlobalVariable* GV = new llvm::GlobalVariable(*rdata.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,VAL);
		((llvm::Value*)GV)->setName(llvm::Twine(variable.getFullName()));
		variable.getMetadata().setObject(finished=new LocationData(loc=new StandardLocation(true,GV),returnType));
		return loc;
	}
public:
	const LocationData* evaluate(RData& r) const final override{
		if(finished){
			if(value){
				Location* aloc = finished->getMyLocation();
				assert(aloc);
				llvm::Value* nex = value->evaluate(r)->castToV(r, returnType, filePos);
				assert(nex);
				aloc->setValue(nex,r);
				incrementCount(r, filePos, finished);
			}
			return finished;
		}
		getReturnType();
		assert(returnType);
		const Data* D = (value==NULL || value->getToken()==T_VOID)?NULL:value->evaluate(r);
		if(isReference==1){
			if(!D){
				filePos.error("Cannot declare reference without value");
				exit(1);
			}
			auto RT = D->getReturnType();
			if(RT->classType!=CLASS_REF){
				filePos.error("Cannot create reference of non-reference type "+RT->getName());
				exit(1);
			}
			const ReferenceData* R = (const ReferenceData*)D;

			if(global) finished = new LocationData(R->value->value->getGlobal(r), returnType);
			else finished = R->value;

			variable.getMetadata().setObject(finished);
			filePos.warning("Garbage collection of references not implemented yet");
			return finished;
		}
		llvm::Value* tmp = (value==NULL || value->getToken()==T_VOID)?NULL:(D->castToV(r, returnType, filePos));
		assert(returnType->type);
		assert(!tmp || tmp->getType()==returnType->type);
		if(global){
			llvm::GlobalVariable* GV;
			if(auto cons = llvm::dyn_cast_or_null<llvm::Constant>(tmp))
				GV = new llvm::GlobalVariable(*r.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,cons);
			else{
				llvm::Constant* VAL;
				if(returnType->layout==POINTER_LAYOUT){
					assert(returnType->type);
					assert(llvm::dyn_cast<llvm::PointerType>(returnType->type));
					VAL = llvm::ConstantPointerNull::get((llvm::PointerType*)returnType->type);
				}
				else
					VAL = r.getGlobal(returnType->type,false);
				GV = new llvm::GlobalVariable(*r.lmod, returnType->type,false, llvm::GlobalValue::PrivateLinkage,VAL);
				if(tmp!=NULL) r.builder.CreateStore(tmp,GV);
			}
			((llvm::Value*)GV)->setName(llvm::Twine(variable.getFullName()));
			variable.getMetadata().setObject(finished=new LocationData(new StandardLocation(true,GV),returnType));
		}
		else{
			auto al = r.createAlloca(returnType->type);
			variable.getMetadata().setObject(finished=new LocationData(getLazy(false,variable.pointer.name,r,al,(tmp)?r.builder.GetInsertBlock():nullptr,tmp),returnType));
		}
		//todo check lazy for globals

		incrementCount(r, filePos, finished);
		return finished;
	}
};
#endif /* Declaration_HPP_ */
