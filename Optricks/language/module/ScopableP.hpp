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
#include "../data/ClassFunctionData.hpp"
#include "../class/builtin/ClassClass.hpp"

// first is one actually there, second is cache
const Data* Scopable::fixClosure(PositionID id, std::pair<std::map<llvm::Value*,llvm::PHINode*>, llvm::BasicBlock*>* tp, const Data* dat){
	switch(dat->type){
		case R_FLOAT:
		case R_INT:
		case R_MATH:
		case R_RATIONAL:
		case R_IMAG:
		case R_STR:
		case R_CLASS:
		case R_GEN:
		case R_NULL:
		case R_FUNC:
		case R_CLASSTEMPLATE:
			return dat;
		case R_ARRAY:{
			auto d = (ArrayData*)dat;
			unsigned i;
			const Data* fv=nullptr;
			for(i=0; i<d->inner.size(); i++){
				auto m=fixClosure(id, tp, d->inner[i]);
				if(m!=d->inner[i]){
					fv = m;
					break;
				}
			}
			if(fv==nullptr) return dat;
			std::vector<const Data* > V;
			for(unsigned j=0; j<i; j++)
				V.push_back(d->inner[i]);
			V.push_back(fv);
			for(unsigned j=i+1; j<d->inner.size(); j++)
				V.push_back(fixClosure(id, tp, d->inner[i]));
			assert(V.size()==d->inner.size());
			return new ArrayData(V, id);
		}
		case R_CLASSFUNC:{
			auto d = (ClassFunctionData*)dat;
			auto m=fixClosure(id, tp, d->instance);
			if(m==d->instance) return dat;
			else return new ClassFunctionData(m, d->function, d->t_args);
		}
		case R_CONST:{
			auto d = (ConstantData*)dat;
			if(llvm::isa<llvm::Constant>(d->value)){
				return dat;
			} else {
				auto find = tp->first.find(d->value);
				if(find!=tp->first.end()){
					return new ConstantData(find->second, d->getReturnType());
				} else {
					auto PN = rdata.builder.CreatePHI(d->value->getType(), 0);
					tp->first.insert(std::pair<llvm::Value*,llvm::PHINode*>(d->value, PN));
					return new ConstantData(PN, d->getReturnType());
				}
			}
		}
		case R_DEC:
		case R_LOC:{
			Location* L;
			if(dat->type==R_DEC){
				auto d = (DeclarationData*)dat;
				assert(d);
				L = d->value->fastEvaluate();
			} else {
				assert(dat->type==R_LOC);
				auto d = (LocationData*)dat;
				assert(d);
				L = d->value;
			}
			assert(L);
			if(L->isGlobal){
				return dat;
			}
			//removed check llvm::isa<llvm::Constant> of rawpointer
			auto PARENT = rdata.builder.GetInsertBlock();
			rdata.builder.SetInsertPoint(tp->second);
			auto V = L->getValue(rdata, id);
			assert(PARENT);
			rdata.builder.SetInsertPoint(PARENT);
			if(llvm::isa<llvm::Constant>(V)){
				return new ConstantData(V, dat->getReturnType());
			} else if(llvm::isa<llvm::LoadInst>(V) && llvm::isa<llvm::Constant>(((llvm::LoadInst*)V)->getPointerOperand()) &&
					L->isPointerEqual( ((llvm::LoadInst*)V)->getPointerOperand() ) ){
				assert(L);
				//assert(dynamic_cast<LazyLocation*>(L)==nullptr);
				return dat;
			} else {
				auto find = tp->first.find(V);
				if(find!=tp->first.end()){
					return new ConstantData(find->second, dat->getReturnType());
				} else {
					//if type of left != right then right should be load of left
					auto PN = rdata.builder.CreatePHI(V->getType(), 0);
					tp->first.insert(std::pair<llvm::Value*,llvm::PHINode*>(V, PN));
					return new ConstantData(PN, dat->getReturnType());
				}
			}
		}
		case R_LAZY:{
			assert(0 && "LAZY SHOULDN'T HAPPEN");
			id.fatalError("Lazy shouldn't be inside closure like that?");
			exit(1);
		}
		case R_MAP:{
			auto d = (MapData*)dat;
			unsigned i;
			const Data* fv=nullptr;
			const Data* f2=nullptr;
			for(i=0; i<d->inner.size(); i++){
				auto m=fixClosure(id, tp, d->inner[i].first);
				if(m!=d->inner[i].first){
					fv = m;
					break;
				}
				auto n=fixClosure(id, tp, d->inner[i].second);
				if(n!=d->inner[i].second){
					f2 = n;
					break;
				}
			}
			if(fv==nullptr && f2==nullptr) return dat;
			std::vector<std::pair<const Data*,const Data*> > V;
			for(unsigned j=0; j<i; j++)
				V.push_back(d->inner[i]);
			V.push_back(std::pair<const Data*,const Data*>(fv?fv:d->inner[i].first, f2?f2:fixClosure(id, tp, d->inner[i].second)));
			for(unsigned j=i+1; j<d->inner.size(); j++)
				V.push_back(std::pair<const Data*,const Data*>(fixClosure(id, tp, d->inner[i].first),fixClosure(id, tp, d->inner[i].second)));
			assert(V.size()==d->inner.size());
			return new MapData(V, d->filePos);
		}
		case R_REF:{
			assert(0 && "REF SHOULDN'T HAPPEN");
			id.fatalError("Ref shouldn't be inside closure like that?");
			exit(1);
		}
		case R_TUPLE:{
			auto d = (TupleData*)dat;
			unsigned i;
			const Data* fv=nullptr;
			for(i=0; i<d->inner.size(); i++){
				auto m=fixClosure(id, tp, d->inner[i]);
				if(m!=d->inner[i]){
					fv = m;
					break;
				}
			}
			if(fv==nullptr) return dat;
			std::vector<const Data* > V;
			for(unsigned j=0; j<i; j++)
				V.push_back(d->inner[i]);
			V.push_back(fv);
			for(unsigned j=i+1; j<d->inner.size(); j++)
				V.push_back(fixClosure(id, tp, d->inner[i]));
			assert(V.size()==d->inner.size());
			return new TupleData(V);
		}
		case R_VOID:{
			assert(0 && "Void shouldn't happen?");
			return dat;
		}

		//todo slice data not implemented
		case R_SLICE:
			return dat;
	}
}
const AbstractClass* Scopable::getClass(PositionID id, const String name, const T_ARGS& t_args) const{
	auto f = find(id,name);
	if(f.first==nullptr) return &voidClass;
	switch(f.second->second.type){
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			return f.first->vars[f.second->second.pos]->getMyClass(id);
		}
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos]->getMyClass(id);
			else
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(id), false)->getMyClass(id);
		}
		case SCOPE_CLASS:
			return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
			return f.first->vars[f.second->second.pos]->getMyClass(id);
		}
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return f.first->funcs[f.second->second.pos]->getMyClass(id);
			else
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(id), false)->getMyClass(id);
		}
		case SCOPE_CLASS:
			return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
	auto dat = f.first->vars[f.second->second.pos];
	//todo fasfds closure
	auto tmp = this;
	decltype(tmp->closureInfo) clos=nullptr;
	while(true){
		assert(tmp);
		if(tmp==f.first){
			break;
		} else if(tmp->closureInfo){
			clos = tmp->closureInfo;
			break;
		}
		tmp = tmp->surroundingScope;
	}
	if(clos){
		assert(tmp->closureInfo);
		//id.warning("FIXING CLOSURE FOR(1): " + name);
		return Scopable::fixClosure(id, tmp->closureInfo, dat);
	} else return dat;
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
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(id), false)->getReturnType();
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
				return f.first->funcs[f.second->second.pos]->getBestFit(id, t_args.eval(id), false)->getReturnType();
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
				return d.first->funcs[d.second->second.pos]->getBestFit(filePos, t_args.eval(filePos), false)->getReturnType();
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

const Data* Resolvable::getObject(const T_ARGS& t_args) const{
	auto d = module->find(filePos,name);
	if(d.first==nullptr) return &VOID_DATA;
	switch(d.second->second.type){
		case SCOPE_FUNC:{
			if(!t_args.inUse)
				return d.first->funcs[d.second->second.pos];
			else
				return d.first->funcs[d.second->second.pos]->getBestFit(filePos, t_args.eval(filePos), false);
		}
		case SCOPE_CLASS:{
			if(t_args.inUse)
				return d.first->classes[d.second->second.pos]->resolveClass(filePos, t_args.eval(filePos));
			else
				return d.first->classes[d.second->second.pos]->resolveClass(filePos, {});
		}
		case SCOPE_VAR:{
			assert(t_args.inUse==false);
			auto dat = d.first->vars[d.second->second.pos];
			//todo closure!
			auto tmp = module;
			decltype(tmp->closureInfo) clos=nullptr;
			while(true){
				assert(tmp);
				if(tmp==d.first){
					break;
				} else if(tmp->closureInfo){
					clos = tmp->closureInfo;
					break;
				}
				tmp = tmp->surroundingScope;
			}
			if(clos){
				//filePos.warning("FIXING CLOSURE FOR(2): " + name);
				return Scopable::fixClosure(filePos, clos, dat);
			} else return dat;
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
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false);
			ret = d->getSingleProto()->returnType;
			break;
		}
		case SCOPE_CLASS:{
			ret = f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false);
			ret = d->getSingleProto()->returnType;
			break;
		}
		case SCOPE_CLASS:{
			ret = f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
				d = f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false);
			ret = std::pair<const Data*,SCOPE_TYPE>(d, SCOPE_FUNC);
			break;
		}
		case SCOPE_CLASS:{
			auto d = f.first->classes[f.second->second.pos];
			const AbstractClass* R;
			if(t_args.inUse) R = d->resolveClass(id, t_args.eval(id));
			else R = d->resolveClass(id, {});
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

bool Scopable::hasCastValue(PositionID id, const String name, const T_ARGS& t_args,const AbstractClass* const a) const{
	auto f = find2(id,name);
	if(f.first==nullptr) return false;
	switch(f.second->second.type){
	case SCOPE_VAR:{
		assert(t_args.inUse==false);
		auto dat = f.first->vars[f.second->second.pos];
		return dat->hasCastValue(a);
	}
	case SCOPE_FUNC:
		if(!t_args.inUse)
			return f.first->funcs[f.second->second.pos]->hasCastValue(a);
		 else
			return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false)->hasCastValue(a);
	case SCOPE_CLASS:
		if(t_args.inUse)
			return f.first->classes[f.second->second.pos]->resolveClass(id, {})->hasCastValue(a);
		else return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id))->hasCastValue(a);
	default:
		return false;
	}
}

int Scopable::compareValue(PositionID id, const String name, const T_ARGS& t_args,const AbstractClass* const a, const AbstractClass* const b) const{
	auto f = find2(id,name);
	if(f.first==nullptr) return 0;
	switch(f.second->second.type){
	case SCOPE_VAR:{
		assert(t_args.inUse==false);
		auto dat = f.first->vars[f.second->second.pos];
		return dat->compareValue(a,b);
	}
	case SCOPE_FUNC:
		if(!t_args.inUse)
			return f.first->funcs[f.second->second.pos]->compareValue(a,b);
		 else
			return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false)->compareValue(a,b);
	case SCOPE_CLASS:
		if(t_args.inUse)
			return f.first->classes[f.second->second.pos]->resolveClass(id, {})->compareValue(a,b);
		else return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id))->compareValue(a,b);
	default:
		return 0;
	}
}

const Data* Scopable::get(PositionID id, const String name, const T_ARGS& t_args) const{
	auto f = find(id,name);
	if(f.first==nullptr) return &VOID_DATA;
	switch(f.second->second.type){
	case SCOPE_VAR:{
		assert(t_args.inUse==false);
		auto dat = f.first->vars[f.second->second.pos];
		//todo fdasfdsa closure
		auto tmp = this;
		decltype(tmp->closureInfo) clos=nullptr;
		while(true){
			assert(tmp);
			if(tmp==f.first){
				break;
			} else if(tmp->closureInfo){
				clos = tmp->closureInfo;
				break;
			}
			tmp = tmp->surroundingScope;
		}
		if(clos){
			assert(tmp->closureInfo);
			//id.warning("FIXING CLOSURE FOR(3): " + name);
			return Scopable::fixClosure(id, tmp->closureInfo, dat);
		} else return dat;
	}
	case SCOPE_FUNC:
		if(!t_args.inUse)
			return f.first->funcs[f.second->second.pos];
		 else
			return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false);
	case SCOPE_CLASS:
		if(t_args.inUse)
			return f.first->classes[f.second->second.pos]->resolveClass(id, {});
		else return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
				return f.first->funcs[f.second->second.pos]->getBestFit(id,t_args.eval(id),false);
		case SCOPE_CLASS:
			if(t_args.inUse)
				return f.first->classes[f.second->second.pos]->resolveClass(id, {});
			else return f.first->classes[f.second->second.pos]->resolveClass(id, t_args.eval(id));
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
		return getObject(t_args)->getValue(r,filePos);
	}
	inline void Resolvable::setValue(RData& r, Data* d2) const{
		assert(module);
		auto d = module->find(filePos,name);
		if(d.first==nullptr) return;
		switch(d.second->second.type){
			case SCOPE_VAR:{
				//TODO fdasfdas closure
				const Data* dat= d.first->vars[d.second->second.pos];
				auto tmp = (d.first==module)?nullptr:module;
				while(tmp!=nullptr){
					if(tmp->closureInfo) break;
					else if(tmp==d.first){
						tmp = nullptr;
						break;
					}
					tmp = tmp->surroundingScope;
				}
				if(tmp){
					assert(tmp->closureInfo);
					//filePos.warning("FIXING CLOSURE FOR(4): " + name);
					dat = Scopable::fixClosure(filePos, tmp->closureInfo, dat);
				}
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
		assert(module);
		auto d = module->find(filePos,name);
		if(d.first==nullptr) return;
		switch(d.second->second.type){
			case SCOPE_VAR:{
				//TODO fdasfdas closure
				const Data* dat= d.first->vars[d.second->second.pos];
				auto tmp = (d.first==module)?nullptr:module;
				while(tmp!=nullptr){
					if(tmp->closureInfo) break;
					else if(tmp==d.first){
						tmp = nullptr;
						break;
					}
					tmp = tmp->surroundingScope;
				}
				if(tmp){
					assert(tmp->closureInfo);
					//filePos.warning("FIXING CLOSURE FOR(5): " + name);
					dat = Scopable::fixClosure(filePos, tmp->closureInfo, dat);
				}
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
		assert(module);
		module->addFunction(filePos,name)->add(d, filePos);
	}
	inline const AbstractClass* Resolvable::getClass(const T_ARGS& args) const{
		assert(module);
		return module->getClass(filePos, name, args);
	}
	inline void Resolvable::setFunction(SingleFunction* d) const{
		assert(module);
		module->addFunction(filePos,name)->set(d,filePos);
	}
	//const AbstractClass* Resolvable::getFunctionReturnType(const std::vector<const AbstractClass*>& fp) const{
	//	return module->getFunctionReturnType(filePos,name,fp);
	//}
	const AbstractClass* Resolvable::getFunctionReturnType(const T_ARGS& args, const std::vector<const Evaluatable*>& fp) const{
		assert(module);
		return module->getFunctionReturnType(filePos,name,args, fp);
	}
	std::pair<const Data*,SCOPE_TYPE> Resolvable::getFunction(const String name, const T_ARGS& args, const std::vector<const AbstractClass*>& fp) const{
		assert(module);
		return module->getFunction(filePos,name,args, fp);
	}
#endif /* SCOPABLEP_HPP_ */
