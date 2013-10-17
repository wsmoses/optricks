/*
 * all.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef ALL_HPP_
#define ALL_HPP_

#include "settings.hpp"
#include "ClassProto.hpp"
#include "FunctionProto.hpp"
#include "GenericClass.hpp"
#include "operations.hpp"
#include "RData.hpp"
#include "types.hpp"
#include "operators.hpp"
#include "basic_functions.hpp"

#ifndef REFERENCEELEM_C_
#define REFERENCEELEM_C_
class ReferenceElement;
#endif
class Resolvable{
	public:
		OModule* module;
		String name;
		Resolvable(OModule* m, String n):module(m),name(n){};
		virtual ~Resolvable(){};
		virtual ReferenceElement* resolve() = 0;
};


void funcMap::add(FunctionProto* f, DATA t, PositionID id){
	if(f==NULL) todo("Error can't add NULL function proto",id);
	if(set(f,t)){
		todo("Error overwriting function "+f->toString(),id);
	}
}
bool funcMap::set(FunctionProto* in, DATA t){

	for(unsigned int i = 0; i<data.size(); i++){
	//	cout << data.size() <<" " << i << endl << flush;
		if(data[i].second->equals(in)){
			data[i].first = t;
			return true;
		}
	}
	data.push_back(std::pair<DATA,FunctionProto*>(t,in));
	return false;
}
std::pair<DATA,FunctionProto*> funcMap::get(FunctionProto* func,PositionID id) const{
	if(func==NULL) todo("NULL FunctionProto",id);
	std::vector<std::pair<std::pair<DATA,FunctionProto*>,std::pair<bool,std::pair<unsigned int, unsigned int> > > > possible;
	int bestind = -1;
	unsigned int count = UINT_MAX;
	unsigned int toCast = UINT_MAX;
	for(unsigned int i = 0; i<data.size(); ++i){
		auto temp = func->match(data[i].second);
		//cout << i << "*" << func->toString()+"|"+data[i].second->toString() << ":" << temp.first << "," << temp.second.first << "," << temp.second.second << endl << flush;
		if(temp.first){
			possible.push_back(std::pair<std::pair<DATA,FunctionProto*>,std::pair<bool,std::pair<unsigned int, unsigned int> > >
			(data[i],temp));
			if(temp.second.first < count){
				bestind = i;
				count = temp.second.first;
				toCast = temp.second.second;
			} else if(temp.second.first==count){
				if(temp.second.second<toCast){
					bestind = i;
					count = temp.second.first;
					toCast = temp.second.second;
				} else if(temp.second.second==toCast){
					bestind = -1;
				}
			}
		}

	}
	if(possible.size()==0){
		String t = "No matching function for "+func->toString()+" options are:\n";
		for(auto & a:data){
			t+=a.second->toString()+"\n";
		}
		todo(t,id);
	}
	if(bestind==-1){
		String t = "Ambiguous function for "+func->toString()+" options are:\n";
		for(unsigned int i=0; i<data.size(); i++){
			if(possible[i].second.second.first == count && possible[i].second.second.second == toCast)
				t+=data[i].second->toString()+"\n";
		}
		todo(t,id);
	}
	return data[bestind];
}

#define REFERENCEELEM_C_
class ReferenceElement:public Resolvable{
	private:
	public:
		DATA llvmObject;
		ClassProto* returnClass;
		funcMap funcs;
		virtual ~ReferenceElement(){};
		ReferenceElement(String c, OModule* mod, String index, DATA value, ClassProto* cl, funcMap fun):
			Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun)
		{
		}
		ReferenceElement(OModule* mod, String index, DATA value, ClassProto* cl, funcMap& fun):
			Resolvable(mod,index), llvmObject(value), returnClass(cl), funcs(fun)
		{
		}
		ReferenceElement* resolve() override final{
			return this;
		}
		Value* getValue(RData& r/*,FunctionProto* func=NULL*/) const{
			return llvmObject.getValue(r);
		}
		//void ensureFunction(FunctionProto* func){

		//	}
		void setValue(DATA d, RData& r/*,FunctionProto* func=NULL*/){
			llvmObject.setValue(r,d.getValue(r));
		}
		void setValue(Value* d, RData& r/*,FunctionProto* func=NULL*/){
			llvmObject.setValue(r,d);
		}
};

ReferenceElement* ClassProto::addFunction(String nam, PositionID id){
	if(nam==name) todo("Cannot make function with same name as class "+name,id);
	if(innerDataIndex.find(nam)!=innerDataIndex.end()) todo("Cannot create another function type for class "+name+" named "+nam,id);
	if(functions.find(nam)==functions.end()) return functions[nam] = new ReferenceElement("",NULL,name+"."+nam,DATA::getFunction(NULL),functionClass,funcMap());
	return functions[nam];
}
#endif /* ALL_HPP_ */
