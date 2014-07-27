/*
 * ScopableP.hpp
 *
 *  Created on: Jan 17, 2014
 *      Author: Billy
 */

#ifndef SCOPABLEP_HPP_
#define SCOPABLEP_HPP_

#include "Scopable.hpp"
#include "../class/AbstractClass.hpp"
#include "../function/AbstractFunction.hpp"
#include "../FunctionProto.hpp"
#include "../data/LocationData.hpp"
#include "../class/builtin/ClassClass.hpp"

const AbstractClass* Scopable::getClass(PositionID id, const String name, const T_ARGS& t_args) const{
	auto f = find(id,name);
	if(f.first==nullptr) return &voidClass;
	switch(f.second->second.type){
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos]->getMyClass(getRData(), id);
		}
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos]->getMyClass(getRData(), id);
			else
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(getRData(), id), false)->getMyClass(getRData(), id);
		}
		case SCOPE_CLASS:
			return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
		default:
			id.error(name+" found at current scope, but was not class");
			exit(1);
	}
}
const AbstractClass* Scopable::getClassHere(PositionID id, const String name, const T_ARGS& t_args) const{
	auto f = findHere(id,name);
	if(f.first==nullptr) return &voidClass;
	switch(f.second->second.type){
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos]->getMyClass(getRData(), id);
		}
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos]->getMyClass(getRData(), id);
			else
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(getRData(), id), false)->getMyClass(getRData(), id);
		}
		case SCOPE_CLASS:
			return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
		default:
			id.error(name+" found at current scope, but was not class");
			exit(1);
	}
}

void Scopable::setVariable(PositionID id, const String name, const Data* da){
	auto d = find(id,name);
	if(d.first==nullptr) return;
	switch(d.second->second.type){
		case SCOPE_VAR:
			d.first->vars[d.second->second.pos] = da;
			break;
		default:
			id.error("Unknown variable type getObject");
			exit(1);
	}
}

const Data* Scopable::getVariable(PositionID id, const String name) const{
	auto f = find(id,name);
	if(f.first==nullptr) return &VOID_DATA;
	if(f.second->second.type!=SCOPE_VAR) id.error(name+" found at current scope, but not correct variable type -- needed non-class variable");
	return f.first->vars[f.second->second.pos];
}
void Scopable::addClass(PositionID id, AbstractClass* c){
	addClass(id, c, c->name);
}
void Scopable::addClass(PositionID id, const MetaClass* c, String s){
	if(existsHere(s)) id.error("Cannot define class "+s+" -- identifier already used at this scope");
	mapping.insert(std::pair<String,SCOPE_POS>(s,SCOPE_POS(SCOPE_CLASS,classes.size())));
	classes.push_back(c);
//	return ( classes.back() );
}

	const AbstractClass* Scopable::getReturnClass(PositionID id, const String name, const T_ARGS& t_args) const{
		auto f = find(id,name);
		if(f.first==nullptr) return &voidClass;
		switch(f.second->second.type){
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos]->getReturnType();
		}
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos]->getReturnType();
			else
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(getRData(), id), false)->getReturnType();
		}
		case SCOPE_CLASS:
			return &classClass;//classClass
		default:
			id.error(name+" found at current scope, but was not static -- needed static variable/class");
			exit(1);
		}
	}

	const AbstractClass* Scopable::getReturnClassHere(PositionID id, const String name, const T_ARGS& t_args) const{
			auto f = findHere(id,name);
			if(f.first==nullptr) return &voidClass;
			switch(f.second->second.type){
			case SCOPE_VAR:{
				assert(t_args.inUse==false);
				return f.first->vars[f.second->second.pos]->getReturnType();
			}
			case SCOPE_FUNC:{
				if(!t_args.inUse)
					return f.first->funcs[f.second->second.pos]->getReturnType();
				else
					return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(getRData(), id), false)->getReturnType();
			}
			case SCOPE_CLASS:
				return &classClass;//classClass
			default:
				id.error(name+" found at current scope, but was not static -- needed static variable/class");
				exit(1);
			}
		}

const AbstractClass* Resolvable::getReturnType(const T_ARGS& t_args) const{
	auto d = module->find(filePos,name);
	if(d.first==nullptr) return &voidClass;
	switch(d.second->second.type){
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return d.first->funcs[d.second->second.pos]->getReturnType();
			else
				return d.first->funcs[d.second->second.pos]->getBestFit(filePos, t_args.eval(getRData(), filePos), false)->getReturnType();
		}
		case SCOPE_CLASS:{
			return &classClass;
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return d.first->vars[d.second->second.pos]->getReturnType();
		}
		default:
			filePos.error("Unknown variable type");
			exit(1);
	}
}
const Data* Resolvable::getObject(RData& r, const T_ARGS& t_args) const{
	auto d = module->find(filePos,name);
	if(d.first==nullptr) return &VOID_DATA;
	switch(d.second->second.type){
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return d.first->funcs[d.second->second.pos];
			else
				return d.first->funcs[d.second->second.pos]->getBestFit(filePos, t_args.eval(r, filePos), false);
		}
		case SCOPE_CLASS:{
			if(t_args.inUse)
				return d.first->classes[d.second->second.pos]->resolveClass(r, filePos, t_args.eval(r, filePos));
			else
				return d.first->classes[d.second->second.pos]->resolveClass(r, filePos, {});
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return d.first->vars[d.second->second.pos];
		}
		default:
			filePos.error("Unknown variable type getObject");
			exit(1);
	}
}
void Resolvable::setObject(const Data* da) const{
	auto d = module->find(filePos,name);
	if(d.first==nullptr) return;
	switch(d.second->second.type){
		case SCOPE_VAR:
			d.first->vars[d.second->second.pos] = da;
			break;
		default:
			filePos.error("Unknown variable type getObject");
			exit(1);
	}
}


/*const AbstractClass* Scopable::getFunctionReturnType(PositionID id, const String name, const std::vector<const AbstractClass*>& fp) const{
	auto f = find(id,name);
	switch(f.second->second.type){
	case SCOPE_FUNC:{
		SingleFunction* d = f.first->funcs[f.second->second.pos]->getBestFit(id,fp);
		return d->getSingleProto()->returnType;
	}
	case SCOPE_CLASS:{
		return f.first->classes[f.second->second.pos];
	}
	case SCOPE_VAR:{
		auto P = f.first->vars[f.second->second.pos];
		return P->getFunctionReturnType(id,fp);
	}
	}
}*/
const AbstractClass* Scopable::getFunctionReturnType(PositionID id, const String name, const T_ARGS& t_args, const std::vector<const Evaluatable*>& fp) const{
	auto f = find(id,name);
	if(f.first==nullptr) return &voidClass;
	const AbstractClass* ret;
	switch(f.second->second.type){
		case SCOPE_FUNC:{
			SingleFunction* d;
			if(!t_args.inUse){
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,fp,nullptr);
			} else
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(getRData(), id),false);
			ret = d->getSingleProto()->returnType;
			break;
		}
		case SCOPE_CLASS:{
			ret = f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
			break;
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			auto P = f.first->vars[f.second->second.pos];
			ret = P->getFunctionReturnType(id,fp,false);
			break;
		}
	}
	return ret;
}
const AbstractClass* Scopable::getFunctionReturnTypeHere(PositionID id, const String name, const T_ARGS& t_args, const std::vector<const Evaluatable*>& fp) const{
	auto f = findHere(id,name);
	if(f.first==nullptr) return &voidClass;
	const AbstractClass* ret;
	switch(f.second->second.type){
		case SCOPE_FUNC:{
			SingleFunction* d;
			if(!t_args.inUse){
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,fp,nullptr);
			} else
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(getRData(), id),false);
			ret = d->getSingleProto()->returnType;
			break;
		}
		case SCOPE_CLASS:{
			ret = f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
			break;
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			auto P = f.first->vars[f.second->second.pos];
			ret = P->getFunctionReturnType(id,fp,false);
			break;
		}
	}
	return ret;
}

inline std::pair<const Data*,SCOPE_TYPE> Scopable::getFunction(PositionID id, const String name, const T_ARGS& t_args, const std::vector<const AbstractClass*>& fp) const{
	auto f = find(id,name);
	if(f.first==nullptr) return std::pair<const Data*,SCOPE_TYPE>(&VOID_DATA,f.second->second.type);
	std::pair<const Data*,SCOPE_TYPE> ret;
	switch(f.second->second.type){
		case SCOPE_FUNC:{
			SingleFunction* d;
			if(!t_args.inUse){
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,fp,nullptr);
			} else
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(getRData(), id),false);
			ret = std::pair<const Data*,SCOPE_TYPE>(d, SCOPE_FUNC);
			break;
		}
		case SCOPE_CLASS:{
			auto d = f.first->classes[f.second->second.pos];
			const AbstractClass* R;
			if(t_args.inUse) R = d->resolveClass(getRData(), id, t_args.eval(getRData(), id));
			else R = d->resolveClass(getRData(), id, {});
			ret = std::pair<const Data*,SCOPE_TYPE>(R, SCOPE_CLASS);
			break;
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			auto P = f.first->vars[f.second->second.pos];
			ret = std::pair<const Data*,SCOPE_TYPE>(P, SCOPE_VAR);
			break;
		}
	}
	return ret;
}

const Data* Scopable::get(PositionID id, const String name, const T_ARGS& t_args) const{
		auto f = find(id,name);
		if(f.first==nullptr) return &VOID_DATA;
		switch(f.second->second.type){
		case SCOPE_VAR:
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos];
		case SCOPE_FUNC:
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos];
			 else
				return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(getRData(), id),false);
		case SCOPE_CLASS:
			//TODO reconsider?
			if(t_args.inUse)
				return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, {});
			else return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
		default:
			id.error(name+" found at current scope, but was not static -- needed static variable/class");
			exit(1);
		}
	}

const Data* Scopable::getHere(PositionID id, const String name, const T_ARGS& t_args) const{
		auto f = findHere(id,name);
		if(f.first==nullptr) return &VOID_DATA;
		switch(f.second->second.type){
		case SCOPE_VAR:
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos];
		case SCOPE_FUNC:
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos];
			 else
				return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(getRData(), id),false);
		case SCOPE_CLASS:
			//TODO reconsider?
			if(t_args.inUse)
				return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, {});
			else return f.first->classes[f.second->second.pos]->resolveClass(getRData(), id, t_args.eval(getRData(), id));
		default:
			id.error(name+" found at current scope, but was not static -- needed static variable/class");
			exit(1);
		}
	}
	OverloadedFunction* Scopable::addFunction(PositionID id, const String name, void* generic){
		auto f = find2Here(id,name);
		if(f.first==NULL){
			mapping.insert(std::pair<const String,SCOPE_POS>(name, SCOPE_POS(SCOPE_FUNC,funcs.size())));
			funcs.push_back(new OverloadedFunction(name, generic));
			return funcs.back();
		}
		else{
			if(f.second->second.type!=SCOPE_FUNC) id.error(name+" found at current scope, but not correct variable type -- needed function");
			return f.first->funcs[f.second->second.pos];
		}
	}

	inline llvm::Value* Resolvable::getValue(RData& r, const T_ARGS& t_args) const{
		return getObject(r, t_args)->getValue(r,filePos);
	}
	inline void Resolvable::setValue(RData& r, Data* d2) const{
		auto d = module->find(filePos,name);
		if(d.first==nullptr) return;
		switch(d.second->second.type){
			case SCOPE_VAR:{
				const Data* dat= d.first->vars[d.second->second.pos];
				if(dat->type==R_LOC)
					((const LocationData*)dat)->setValue(r, d2->getValue(r, filePos));
				else if(dat->type==R_DEC)
					((const DeclarationData*)dat)->setValue(r, d2->getValue(r, filePos));
				else
					filePos.error("Cannot set value of non-variable '"+name+"'");
				return;
			}
			default:
				filePos.error("Cannot set value of non-variable '"+name+"'");
				exit(1);
		}
	}
	inline void Resolvable::setValue(RData& r,llvm::Value* v) const{
		auto d = module->find(filePos,name);
		if(d.first==nullptr) return;
		switch(d.second->second.type){
			case SCOPE_VAR:{
				const Data* dat= d.first->vars[d.second->second.pos];
				if(dat->type==R_LOC)
					((const LocationData*)dat)->setValue(r, v);
				else if(dat->type==R_DEC)
					((const DeclarationData*)dat)->setValue(r, v);
				else
					filePos.error("Cannot set value of non-variable '"+name+"'");
				return;
			}
			default:
				filePos.error("Cannot set value of non-variable '"+name+"'");
				exit(1);
		}
	}
	inline void Resolvable::addFunction(SingleFunction* d) const{
		module->addFunction(filePos,name)->add(d, filePos);
	}
	inline const AbstractClass* Resolvable::getClass(const T_ARGS& args) const{
		return module->getClass(filePos, name, args);
	}
	inline void Resolvable::setFunction(SingleFunction* d) const{
		module->addFunction(filePos,name)->set(d,filePos);
	}
	//const AbstractClass* Resolvable::getFunctionReturnType(const std::vector<const AbstractClass*>& fp) const{
	//	return module->getFunctionReturnType(filePos,name,fp);
	//}
	const AbstractClass* Resolvable::getFunctionReturnType(const T_ARGS& args, const std::vector<const Evaluatable*>& fp) const{
		return module->getFunctionReturnType(filePos,name,args, fp);
	}
	std::pair<const Data*,SCOPE_TYPE> Resolvable::getFunction(const String name, const T_ARGS& args, const std::vector<const AbstractClass*>& fp) const{
		return module->getFunction(filePos,name,args, fp);
	}
#endif /* SCOPABLEP_HPP_ */
