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
#include "../primitives/oobjectproto.hpp"

#define dataType Value*

class Resolvable{
	public:
	OModule* module;
	String name;
	virtual dataType& resolve() const = 0;
	virtual ClassProto*& resolveReturnClass() const = 0;
	virtual FunctionProto*& resolveFunction() const = 0;
	virtual ClassProto*& resolveSelfClass() const = 0;
};

class opointer: public Resolvable{
	public:
		unsigned int index;
		opointer(OModule* a, unsigned int b, String c) : index(b){
			module = a;
			name = c;
		}
		ostream& operator << ( ostream& a){
			a << "(*" << name << "|" << index << ")";
			return a;
		}
		dataType& resolve() const final override;
		ClassProto*& resolveReturnClass() const final override;
		ClassProto*& resolveSelfClass() const final override;
		FunctionProto*& resolveFunction() const final override;
};

class OModule : public Stackable{
	public:
		OModule* super;
		std::map<String, opointer*> mapping;
		std::vector<ClassProto*> returnClasses;
		std::vector<ClassProto*> selfClasses;
		std::vector<FunctionProto*> functions;
		std::vector<dataType> data;
		OModule(const OModule& c) = delete;
		OModule(OModule* before): mapping(),returnClasses(),selfClasses(),data(){
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
		void setPointer(String index, dataType value, ClassProto* cl, FunctionProto* fun, ClassProto* selfCl){
			auto p = findPointer(index);
			p->resolve() = value;
			p->resolveReturnClass() = cl;
			p->resolveFunction() = fun;
			p->resolveSelfClass() = selfCl;
		}
		opointer* addPointer(String index, dataType value, ClassProto* cla, FunctionProto* fun, ClassProto* selfCl, unsigned int level=0){
			if(level == 0){
				if(mapping.find(index)!=mapping.end()) todo("The variable "+index+" has already been defined in this scope");
				opointer* nex = new opointer(this, data.size(), index);
				data.push_back(value);
				returnClasses.push_back(cla);
				functions.push_back(fun);
				selfClasses.push_back(selfCl);
				mapping.insert(std::pair<String,opointer*>(index, nex));
				return nex;
			} else {
				if(super==NULL){
					cerr << "Null module to add pointer to" << endl << flush;
					exit(1);
				}
				else
				return super->addPointer(index, value, cla, fun, selfCl, level-1);
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
			return addPointer(index, NULL,NULL, NULL,NULL);
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
			write(cerr, "");
			cerr << endl << flush;
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
ClassProto*& opointer::resolveReturnClass() const {
	return module->returnClasses[index];
}
FunctionProto*& opointer::resolveFunction() const {
	return module->functions[index];
}
ClassProto*& opointer::resolveSelfClass() const {
	return module->selfClasses[index];
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
		ClassProto*& resolveReturnClass() const override final{
			return resolvePointer()->resolveReturnClass();
		}
		FunctionProto*& resolveFunction() const override final{
			return resolvePointer()->resolveFunction();
		}
		ClassProto*& resolveSelfClass() const override final{
			return resolvePointer()->resolveSelfClass();
		}
};
#undef dataType
OModule* LANG_M = new OModule(NULL);

RData::RData():
			module(new OModule(LANG_M)),
			lmod(new Module("main",getGlobalContext())),
			builder(IRBuilder<>(lmod->getContext()))
			{

	  InitializeNativeTarget();
			exec = EngineBuilder(lmod).create();
			fpm = new FunctionPassManager(lmod);
	fpm->add(new DataLayout(*exec->getDataLayout()));
	// Provide basic AliasAnalysis support for GVN.
	fpm->add(createBasicAliasAnalysisPass());
	// Do simple "peephole" optimizations and bit-twiddling optzns.
	fpm->add(createInstructionCombiningPass());
	// Reassociate Statements.
	fpm->add(createReassociatePass());
	// Eliminate Common SubStatements.
	fpm->add(createGVNPass());
	// Simplify the control flow graph (deleting unreachable blocks, etc).
	fpm->add(createCFGSimplificationPass());

	fpm->doInitialization();
		}
#endif /* MODULE_HPP_ */
