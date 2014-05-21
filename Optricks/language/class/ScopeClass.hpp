/*
 * ScopeClass.hpp
 *
 *  Created on: May 8, 2014
 *      Author: Billy
 */

#ifndef SCOPECLASS_HPP_
#define SCOPECLASS_HPP_

#include "./AbstractClass.hpp"
#include "../function/UnaryFunction.hpp"
class ScopeClass: public AbstractClass{
private:
public:
	friend AbstractClass;
	ScopeClass(Scopable* sc, PositionID id, String nam):
		AbstractClass(sc, nam, nullptr, LITERAL_LAYOUT, CLASS_SCOPE, true, VOIDTYPE, sc)
	{
		sc->addClass(id, this);
	}
	/*
	inline OverloadedFunction* addLocalFunction(const String s, void* generic=nullptr){
		auto find = localFunctions.find(s);
		if(find==localFunctions.end()){
			return localFunctions.insert(
					std::pair<String,OverloadedFunction*>(s,new OverloadedFunction(s, generic))).first->second;
		}
		return find->second;
	}
	inline const UnaryFunction& getPostop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->postop.find(s);
			if(find!=tmp->postop.end()) return find->second;
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find unary post-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const UnaryFunction& getPreop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->preop.find(s);
			if(find!=tmp->preop.end()) return find->second;
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find unary pre-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const AbstractClass*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) return find->second->getBestFit(id,v);
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}

	inline Value* generateData(RData& r, PositionID id) const;
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) return find->second->getBestFit(id,v);
			tmp = (const UserClass*)superClass;
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	void finalize(PositionID id){
		assert(!final);
		final = true;
		if(superClass){
			UserClass* uc = (UserClass*)superClass;
			if(!uc->final) id.compilerError("Superclass is not finalized");
			start = uc->start+uc->localVars.size();
		} else start = 0;
		StructType* structType = (layout==POINTER_LAYOUT)?(
			(StructType*)(((PointerType*)type)->getArrayElementType())):
		((StructType*)type);
		int counter = start+localVars.size();
		llvm::SmallVector<Type*,0> types(counter);
		counter--;
		UserClass* tmp = this;
		do{
			const auto at=tmp->localVars.size();
			if(at >0){
			for(unsigned int i=at-1; ; i--){
				assert(counter>=0);
				assert(counter<types.size());
				types[counter] = tmp->localVars[i]->type;
				counter--;
				if(i==0) break;
			}
			}
			tmp = (UserClass*) tmp->superClass;
		}while(tmp);
		assert(counter==-1);
		for(unsigned i=0; i<types.size();i++){
			assert(types[i]);
			//types[i]->dump();
			//cerr << endl << flush;
		}
		structType->setBody(types,false);
	}
	void addLocalVariable(PositionID id, String s, const AbstractClass* const ac){
		assert(ac);
		if(ac->layout==LITERAL_LAYOUT){
			id.error("Cannot store literal data as class variable");
			return;
		}
		if(ac->classType==CLASS_REF){
			id.error("Cannot store reference as class variable");
			return;
		}
		if(ac->classType==CLASS_LAZY){
			id.error("Cannot store lazy data as class variable");
			return;
		}
		auto tmp=this;
		do{
			if(tmp->localMap.find(s)!=tmp->localMap.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a variable in '"+tmp->getName()+"'");
				return;
			}
			if(tmp->localFunctions.find(s)!=tmp->localFunctions.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a function in '"+tmp->getName()+"'");
				return;
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		//if(staticVariables.exists(s)){}
		localMap[s]=localVars.size();
		localVars.push_back(ac);
	}
	*/
	bool hasLocalData(String s) const override final{
		return false;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
			illegalLocal(id,s);
			exit(1);
	}

	bool noopCast(const AbstractClass* const toCast) const override{
		return false;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return hasCast(toCast);
	}
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		id.compilerError("Scope class cannot be instantiated --- how did this even happen....?");
		exit(1);
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		return 0;
	}
};

ScopeClass NS_LANG(&LANG_M, PositionID("#init",0,0), "lang");
ScopeClass NS_LANG_C(&(NS_LANG.staticVariables), PositionID("#init",0,0), "c");
ScopeClass NS_LANG_CPP(&(NS_LANG.staticVariables), PositionID("#init",0,0), "cpp");

#endif /* SCOPECLASS_HPP_ */
