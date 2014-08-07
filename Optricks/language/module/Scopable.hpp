/*
 * Scopable.hpp
 *
 *  Created on: Nov 22, 2013
 *      Author: wmoses
 */

#ifndef SCOPABLE_HPP_
#define SCOPABLE_HPP_

#include "../includes.hpp"
#include "./ClassTemplate.hpp"
//#include "../data/Data.hpp"
//#include "../FunctionProto.hpp"
//#include "../functions/AbstractFunction.hpp"

//todo, since these are all Data*, it is possible to unify / simplify this
enum SCOPE_TYPE{
	SCOPE_FUNC,/*Class method*/
	SCOPE_VAR,/*Class variable*/
	SCOPE_CLASS, /* class */
	//SCOPE_SCOPE /* sub-scope (like nested module) */
};
struct SCOPE_POS{
	SCOPE_TYPE type;
	unsigned int pos;
	SCOPE_POS(SCOPE_TYPE t,unsigned int a):type(t),pos(a){};
};

class MetaClass{
public:
	virtual ~MetaClass(){};
	virtual const AbstractClass* resolveClass(PositionID id, const std::vector<TemplateArg>&) const=0;
};
#define SCOPABLE_C_
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
	std::vector<const MetaClass*> classes;
public:
	virtual ~Scopable(){};
	Scopable(Scopable* above,String n=""):name(n),surroundingScope(above){};
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
		assert(this);
		if(!surroundingScope) return name;
		else{
			String s = surroundingScope->getScopeName();
			if(s.length()==0) return name;
			return  s + "." + name;
		}
	}
	inline std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator> find(PositionID id, const String s) {
		auto tmp = find2(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			write(cerr);
			cerr << endl << flush;
			return tmp;
		} else return tmp;
	}
	inline std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator> find(PositionID id, const String s) const{
		auto tmp = find2(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			write(cerr);
			cerr << endl << flush;
			return tmp;
		} else return tmp;
	}
	inline std::pair<Scopable*,std::map<const String,SCOPE_POS>::iterator> findHere(PositionID id, const String s) {
		auto tmp = find2Here(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			write(cerr);
			cerr << endl << flush;
			return tmp;
		} else return tmp;
	}
	inline std::pair<const Scopable*,std::map<const String,SCOPE_POS>::const_iterator> findHere(PositionID id, const String s) const{
		auto tmp = find2Here(id,s);
		if(tmp.first==NULL){
			id.error("Cannot find "+s+" in current scope");
			write(cerr);
			cerr << endl << flush;
			return tmp;
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
	const AbstractClass* getClass(PositionID id, const String name, const T_ARGS&) const;
	const AbstractClass* getClassHere(PositionID id, const String name, const T_ARGS&) const;

	inline const AbstractClass* getFunctionReturnTypeHere(PositionID id, const String name, const T_ARGS& t_args, const std::vector<const Evaluatable*>& fp) const;
	inline const AbstractClass* getFunctionReturnType(PositionID id, const String name, const T_ARGS& t_args, const std::vector<const Evaluatable*>& fp) const;
	inline std::pair<const Data*,SCOPE_TYPE> getFunction(PositionID id, const String name, const T_ARGS&, const std::vector<const AbstractClass*>& fp) const;

	const Data* getVariable(PositionID id, const String name) const;
	void setVariable(PositionID id, const String name, const Data* da);

	const AbstractClass* getReturnClass(PositionID id, const String name, const T_ARGS&) const;
	const Data* get(PositionID id, const String name, const T_ARGS&) const;

	const AbstractClass* getReturnClassHere(PositionID id, const String name, const T_ARGS&) const;
	const Data* getHere(PositionID id, const String name, const T_ARGS&) const;

	OverloadedFunction* addFunction(PositionID id, const String name, void* generic=nullptr);
	void addClass(PositionID id, AbstractClass* c);
	void addClass(PositionID id, const MetaClass* c,String n="");
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
	OModule(Scopable* m):Scopable(m){};
};

class Resolvable{
public:
	const String name;
	Scopable* module;
	PositionID filePos;
	Resolvable(Scopable* m,const String n, PositionID id): name(n),module(m),filePos(id){
		assert(module);
		assert(n!="auto");
	};
	const AbstractClass* getReturnType(const T_ARGS&) const;
	inline const Data* getObject(const T_ARGS&) const;
	void setObject(const Data* da) const;
	inline llvm::Value* getValue(RData& r, const T_ARGS&) const;
	inline void setValue(RData& r, Data* d2) const;
	inline void setValue(RData& r,llvm::Value* v) const;
	inline void addFunction(SingleFunction* d) const;
	inline void setFunction(SingleFunction* d) const;
	inline const AbstractClass* getClass(const T_ARGS& args) const;
	//const AbstractClass* getFunctionReturnType(const std::vector<const AbstractClass*>& fp) const;
	const AbstractClass* getFunctionReturnType(const T_ARGS&, const std::vector<const Evaluatable*>& fp) const;
	std::pair<const Data*,SCOPE_TYPE> getFunction(const String name, const T_ARGS&, const std::vector<const AbstractClass*>& fp) const;
	/*inline void addLocalFunction(RData& r, PositionID id, DATA d){
			module->addLocalFunction(id,name).add(d, r, id);
		}
		inline void setLocalFunction(RData& r, PositionID id, DATA d){
			module->addLocalFunction(id,name).set(d,r,id);
		}*/
};

OModule LANG_M(NULL);

//Scopable INNER_LANG_(&LANG_M,"lang");
//Scopable INNER_LANG_C_(&INNER_LANG_,"c");
#endif /* SCOPABLE_HPP_ */
