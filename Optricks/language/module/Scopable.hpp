/*
 * Scopable.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: wmoses
 */

#ifndef SCOPABLE_HPP_
#define SCOPABLE_HPP_

#include "../includes.hpp"
//#include "../data/Data.hpp"
//#include "../FunctionProto.hpp"
//#include "../functions/AbstractFunction.hpp"

//todo, since these are all Data*, it is possible to unify / simplify this
enum SCOPE_TYPE{
	SCOPE_FUNC,/*Class method*/
	SCOPE_VAR,/*Class variable*/
	SCOPE_CLASS, /* class */
	SCOPE_SCOPE /* sub-scope (like nested module) */
};
struct SCOPE_POS{
	SCOPE_TYPE type;
	unsigned int pos;
	SCOPE_POS(SCOPE_TYPE t,unsigned int a):type(t),pos(a){};
};

class Scopable{
private:
	const String name;
public:
	Scopable* const surroundingScope;
	//protected:
	std::map<const String,SCOPE_POS> mapping;
	std::vector<OverloadedFunction*> funcs;
	std::vector<const Data*> vars;
	std::vector<Scopable*> scopes;
	std::vector<AbstractClass*> classes;
public:
	virtual ~Scopable(){};
	Scopable(Scopable* above):surroundingScope(above){};
	const bool existsHere(String s) const{
		return mapping.find(s)!=mapping.end();
	}
	const bool exists(String s) const{
		auto tmp = this;
		assert(tmp);
		do{
			if(tmp->mapping.find(s)!=tmp->mapping.end()) return true;
			tmp = tmp->surroundingScope;
		}while(tmp!=NULL);
		return false;
	}
	inline String getScopeName() const{
		if(!surroundingScope) return name;
		else{
			String s = surroundingScope->getScopeName();
			return  s + "." + name;
		}
	}
	inline std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator> find(PositionID id, const String s) {
		auto tmp = find2(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			exit(1);
		} else return tmp;
	}
	inline std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator> find(PositionID id, const String s) const{
		auto tmp = find2(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			exit(1);
		} else return tmp;
	}
	inline SCOPE_TYPE getScopeType(PositionID id, const String s) const{
		auto tmp = find2(id,s);
		return tmp.second->second.type;
	}
	inline std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator> find2Here(PositionID id, const String s) const {
		std::map<const String,SCOPE_POS>::const_iterator it = mapping.find(s);
		if(it!=mapping.end()) return std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator>(this,it);
		return std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator>(nullptr,mapping.end());
	}
	inline std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator> find2Here(PositionID id, const String s) {
		std::map<const String,SCOPE_POS>::iterator it = mapping.find(s);
		if(it!=mapping.end()) return std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator>(this,it);
		return std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator>(nullptr,mapping.end());
	}
	inline std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator> find2(PositionID id, const String s) const {
			const Scopable* tmp = this;
			assert(tmp);
			do{
				std::map<const String,SCOPE_POS>::const_iterator it = tmp->mapping.find(s);
				if(it!=tmp->mapping.end()) return std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator>(tmp,it);
				tmp = tmp->surroundingScope;
			}while(tmp!=NULL);
			return std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator>(nullptr,mapping.end());
		}
	inline std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator> find2(PositionID id, const String s) {
		Scopable* tmp = this;
		assert(tmp);
		do{
			std::map<const String,SCOPE_POS>::iterator it = tmp->mapping.find(s);
			if(it!=tmp->mapping.end()) return std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator>(tmp,it);
			tmp = tmp->surroundingScope;
		}while(tmp!=NULL);
		return std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator>(nullptr,mapping.end());
	}
	const AbstractClass* getClass(PositionID id, const String name) const{
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_CLASS) id.error(name+" found at current scope, but not correct variable type -- needed class");
		return f.first->classes[f.second->second.pos];
	}
	AbstractClass*& getClass(PositionID id, const String name){
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_CLASS) id.error(name+" found at current scope, but not correct variable type -- needed class");
		return f.first->classes[f.second->second.pos];
	}
	OverloadedFunction* getStaticFunction(PositionID id, const String name) const{
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_FUNC) id.error(name+" found at current scope, but not correct variable type -- needed non-class function");
		return f.first->funcs[f.second->second.pos];
	}
	inline const AbstractClass* getFunctionReturnType(PositionID id, const String name, const std::vector<const AbstractClass*>& fp) const;
	inline const AbstractClass* getFunctionReturnType(PositionID id, const String name, const std::vector<const Evaluatable*>& fp) const;
	inline std::pair<SingleFunction*,SCOPE_TYPE> getFunction(PositionID id, const String name, const std::vector<const AbstractClass*>& fp) const;

	const Data* getVariable(PositionID id, const String name) const{
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_VAR) id.error(name+" found at current scope, but not correct variable type -- needed non-class variable");
		return f.first->vars[f.second->second.pos];
	}
	const Data*& getVariable(PositionID id, const String name){
		auto f = find(id,name);
		if(f.second->second.type!=SCOPE_VAR) id.error(name+" found at current scope, but not correct variable type -- needed non-class variable");
		return f.first->vars[f.second->second.pos];
	}

	const AbstractClass* getReturnClass(PositionID id, const String name) const;
	const Data* get(PositionID id, const String name) const;
	OverloadedFunction* addFunction(PositionID id, const String name, void* generic=nullptr);
	void addClass(PositionID id, AbstractClass* c);
	void addVariable(PositionID id, const String name,Data* d){
		if(existsHere(name)) id.error("Cannot define variable "+name+" -- identifier already used at this scope");
		mapping.insert(std::pair<const String,SCOPE_POS>(name,SCOPE_POS(SCOPE_VAR,vars.size())));
		vars.push_back(d);
	}
	virtual void write(ostream& a) const{
		a << "Scope[" << flush;
		bool first = true;
		for(auto & b: mapping){
			if(first) first = false;
			else a << ", " << flush;
			a << b.first << ":";
			//	if(b.second->llvmObject.==NULL)
			//a << "null";
			//else a << b.second->returnClass->name;
		}
		a << "]|" << flush;
		if(surroundingScope!=NULL) surroundingScope->write(a);
	}
};

class OModule:public Scopable{
public:
	OModule(OModule* m):Scopable(m){};
};

class Resolvable{
public:
	const String name;
	Scopable* const module;
	PositionID filePos;
	Resolvable(Scopable* m,const String n, PositionID id): name(n),module(m),filePos(id){};
	const AbstractClass* getReturnType() const;
	inline const Data* getObject() const;
	void setObject(const Data* da) const;
	inline Value* getValue(RData& r) const;
	inline void setValue(RData& r, Data* d2) const;
	inline void setValue(RData& r,Value* v) const;
	inline void addFunction(SingleFunction* d) const;
	inline void setFunction(SingleFunction* d) const;
	inline AbstractClass* getClass() const;
	const AbstractClass* getFunctionReturnType(const std::vector<const AbstractClass*>& fp) const;
	const AbstractClass* getFunctionReturnType(const std::vector<const Evaluatable*>& fp) const;
	std::pair<SingleFunction*,SCOPE_TYPE> getFunction(const String name, const std::vector<const AbstractClass*>& fp) const;
	/*inline void addLocalFunction(RData& r, PositionID id, DATA d){
			module->addLocalFunction(id,name).add(d, r, id);
		}
		inline void setLocalFunction(RData& r, PositionID id, DATA d){
			module->addLocalFunction(id,name).set(d,r,id);
		}*/
};

OModule* LANG_M = new OModule(NULL);
#endif /* SCOPABLE_HPP_ */
