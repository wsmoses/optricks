/*
 * Module.hpp
 * Contains list of local variables for resolution
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef MODULE_HPP_
#define MODULE_HPP_

#include "Stackable.hpp"
#include "../containers/settings.hpp"
#include "../primitives/oobject.hpp"

#define dataType Value*

class Resolvable{
	public:
	OModule* module;
	String name;
	virtual dataType& resolve() const = 0;
	virtual Stackable*& resolveMeta() const = 0;
	virtual Type*& resolveType() const = 0;
};

class opointer: public Resolvable{
	public:
		unsigned int index;
		String name;
		opointer(){

		}
		opointer(OModule* a, unsigned int b, String c) : index(b), name(c){
			module = a;
			cout << "Made pointer to " << name << endl << flush;
		}
		ostream& operator << ( ostream& a){
			a << "(*" << name << "|" << index << ")";
			return a;
		}
		dataType& resolve() const final override;
		Stackable*& resolveMeta() const final override;
		Type*& resolveType() const final override;
};

class OModule : public Stackable{
	public:
		OModule* super;
		std::map<String, opointer*> mapping;
		std::vector<Type*> types;
		std::vector<Stackable*> meta;
		std::vector<dataType> data;
		OModule(const OModule& c) = delete;
		OModule(OModule* before): mapping(),types(),meta(),data(){
			super = before;
		}
		const Token getToken() const override{
			return T_MODULE;
		}
		const int exists(String index) const{
			const OModule* search = this;
			int level = 0;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
					level++;
				} else {
					return level;
				}
			}
			return -1;
		}
		void setPointer(String index, dataType value, Stackable* meta,Type* t){
			findPointer(index)->resolve() = value;
			findPointer(index)->resolveMeta() = meta;
			findPointer(index)->resolveType() = t;
		}
		opointer* addPointer(String index, dataType value, Stackable* met, Type* t, unsigned int level=0){
			if(level == 0){
				if(mapping.find(index)!=mapping.end()) todo("The variable "+index+" has already been defined in this scope");
				opointer* nex = new opointer(this, data.size(), index);
				data.push_back(value);
				meta.push_back(met);
				types.push_back(t);
				mapping.insert(std::pair<String,opointer*>(index, nex));
				return nex;
			} else {
				if(super==NULL){
					cerr << "Null module to add pointer to" << endl << flush;
					exit(1);
				}
				else
				return super->addPointer(index, value, met, t, level-1);
			}
		}
		opointer* findPointer(String index) {
			const OModule* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return paired->second;
				}
			}
			return addPointer(index, NULL,NULL,NULL);
		}
		opointer* getPointer(String index) {
			OModule* search = this;
			while(search!=NULL){
				auto paired = search->mapping.find(index);
				if(paired== search->mapping.end()){
					search = search->super;
				} else {
					return paired->second;
				}
			}
			cerr << "Could not resolve variable: " << index << flush << endl;
			exit(0);
		}
		void write(ostream& a,String t) const override{
			a << "Module[" << flush;
			bool first = true;
			for(auto & b: mapping){
				if(first) first = false;
				else a << ", " << flush;
				a << b.first << flush;
			}
			a << "]|" << flush;
			if(super!=NULL) super->write(a,t);
		}
};

dataType& opointer::resolve() const {
	return module->data[index];
}
Stackable*& opointer::resolveMeta() const {
	return module->meta[index];
}
Type*& opointer::resolveType() const {
	return module->types[index];
}

class LateResolve : public Resolvable{
	public:
		LateResolve(OModule* m,String n){
			name = n;
			module = m;
		}
		opointer* resolvePointer() const{
			auto a =  module->getPointer(name);
			if(a==NULL) todo("Could not resolve late pointer for ", name);
			return a;
		}
		Value*& resolve() const override final{
			return resolvePointer()->resolve();
		}
		Stackable*& resolveMeta() const override final{
			return resolvePointer()->resolveMeta();
		}
		Type*& resolveType() const override final{
			return resolvePointer()->resolveType();
		}
};
#undef dataType
OModule* LANG_M = new OModule(NULL);

RData::RData():
			module(new OModule(LANG_M)),
			builder(IRBuilder<>(getGlobalContext()))
			{
	lmod = new Module("main", getGlobalContext());
		}
#endif /* MODULE_HPP_ */
