/*
 * settings.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_
#define  LOCAL_FUNC Function::PrivateLinkage
#define EXTERN_FUNC Function::ExternalLinkage
#define VERIFY(A)
//#define VERIFY(A) verifyFunction(A);
#include "basic_functions.h"

//#include <GL/glut.h>
#undef VOID
#include <initializer_list>
#include <cstdio>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <array>
#include <unordered_set>

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/CFG.h"
using namespace llvm;
#include <assert.h>
//#define cout std::cout
#define cin std::cin
#define cerr std::cerr
#define flush std::flush
#define endl std::endl << flush
#define String std::string
#define ostream std::ostream
//#define char int
#define byte unsigned short
#define exception std::exception

class PositionID{
	public:
		unsigned int lineN;
		unsigned int charN;
		String fileName;
		PositionID():lineN(0),charN(0),fileName("<c++ pair>"){};
		PositionID(unsigned int a, unsigned int b, String c){
			lineN = a;
			charN = b;
			fileName = c;
		}
		//	PositionID():PositionID(0,0,"<start>"){}
		void error(String s, bool end=true) const{
			cerr << s << " at " << fileName << " on line " << lineN << ", char " << charN << endl << flush;
			if(end){
#ifndef NDEBUG
				assert(0);
#else
				exit(1);
#endif
			}
		}
		ostream& operator << (ostream& o) const{
			o << fileName;
			o << " line:";
			o << lineN;
			o << ", char: ";
			o << charN;
			return o;
		}
};

#ifndef DECLR_P_
#define DECLR_P_
class Declaration;
#endif

class OModule;
#ifndef OOBJECT_P_
#define OOBJECT_P_
class oobject;
#endif
/*
ArrayRef<int> getArrayRefFromString(String s){
	std::vector<int> v;
	for(auto a:s) v.push_back(a);
	return ArrayRef<int>(v);
}*/

#ifndef OPERATIONS_C_
#define OPERATIONS_C_
class obinop;
class ouop;
class obinopNative;
class ouopNative;
#endif

#ifndef REFERENCEELEM_C_
#define REFERENCEELEM_C_
class ReferenceElement;
#endif

#ifndef RDATA_C_
#define RDATA_C_
class RData;
#endif

#ifndef CLASSFUNC_C_
#define CLASSFUNC_C_
class classFunction;
#endif

#ifndef E_FUNC_CALL_C_
#define E_FUNC_CALL_C_
class E_FUNC_CALL;
#endif

#ifndef FUNCTIONPROTO_C_
#define FUNCTIONPROTO_C_
class FunctionProto;
#endif

#ifndef STATEMENT_P_
#define STATEMENT_P_
class Statement;
#endif


#ifndef E_GEN_C_
#define E_GEN_C_
class E_GEN;
#endif

#ifndef CLASSPROTO_C_
#define CLASSPROTO_C_
class ClassProto;
#endif

enum DataType{
	R_LAZY = 6,
	R_CONST = 5,
	R_CLASS = 4,
	R_FUNC = 3,
	R_GEN = 2,
	R_LOC = 1,
	R_UNDEF = 0
};
template<typename C>
String str(C a){
	std::stringstream ss;
	ss << a;
	return ss.str();
}

class LazyLocation;
template<> String str<DataType>(DataType d){
	switch(d){
		case R_LAZY: return "R_LAZY";
		case R_CONST: return "R_CONST";
		case R_CLASS: return "R_CLASS";
		case R_FUNC: return "R_FUNC";
		case R_GEN: return "R_GEN";
		case R_LOC: return "R_LOC";
		case R_UNDEF: return "R_UNDEF";
	}
	return "unknown DATATYPE";
}

class Location{
	public:
		//virtual ~Location()=0;
		virtual Value* getValue(RData& r, PositionID id)=0;
		virtual void setValue(Value* v, RData& r)=0;
		virtual Value* getPointer(RData& r,PositionID id) =0;
};
union PrivateData{
		Value* constant;
		ClassProto* classP;
		Function* function;
		E_GEN* generator;
		Location* location;
		void* pointer;
};
#include "../O_Token.hpp"
#include "types.hpp"
//#include "indexed_map.hpp"

struct ComplexStruct{
		double real, complex;
};

enum LayoutType {
	PRIMITIVE_LAYOUT = 2,
	PRIMITIVEPOINTER_LAYOUT = 1,
	POINTER_LAYOUT = 0
};


template<> String str<LayoutType>(LayoutType d){
	switch(d){
		case PRIMITIVE_LAYOUT: return "PRIMITIVE_LAYOUT";
		case PRIMITIVEPOINTER_LAYOUT: return "PRIMITIVEPOINTER_LAYOUT";
		case POINTER_LAYOUT: return "POINTER_LAYOUT";
	}
	return "unknown LayoutType";
}
template<typename C> bool in(const std::vector<C> a, C b){
	for(const auto& e: a)
		if(e==b) return true;
	return false;
}
template <class T> ostream& operator<<(ostream&os, std::vector<T>& v){
		bool first = true;
		os<<"[";
		for(const auto& a:v){
			if(first){
				first= false;
				os<<a;
			}else os<<", "<<a;
		}
		return os<<"]";
}

#include <unistd.h>

#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
void getDir(String pos, String& dir, String& file){
	size_t p = pos.find_last_of('/');
	if(p==pos.npos) p = pos.find_last_of('\\');
	else{
		size_t tmp = pos.find_last_of('\\');
		if(tmp!=pos.npos && tmp>p) p = tmp;
	}
	if(p==pos.npos){
		dir=".";
		file = pos;
	} else if(p>0 && pos[p-1]=='/'){
		cerr << "Do not accept // filepaths" << endl << flush;
		exit(1);
	} else{
		dir = pos.substr(0,p);
		file = pos.substr(p+1);
	}
}


enum JumpType{
	RETURN = 0,
	BREAK = 1,
	CONTINUE = 2,
	YIELD = 3
};

enum TJump{
	FUNC = 0,
	LOOP = 1,
	GENERATOR = 2
};


union InfoType{
		ClassProto* classType;
		FunctionProto* funcType;
		void* pointer;
};


class DATA{
	private:
		DataType type;
		InfoType info;
		PrivateData data;
		DATA(int i):type(R_UNDEF){data.pointer = NULL;info.pointer = NULL; };
		DATA(DataType t, void* v, void* i):type(t){data.pointer = v;info.pointer = i;};
	public:
		DATA(const DATA& d):type(d.type){assert(d.type<6); data.pointer = d.data.pointer; info.pointer = d.info.pointer;}
		DATA(DATA& d):type(d.type){assert(d.type<6); data.pointer = d.data.pointer; info.pointer = d.info.pointer;}
		DATA castTo(RData& r, ClassProto* right, PositionID id) const;
		inline Value* castToV(RData& r, ClassProto* right, PositionID id) const{
			DATA d = castTo(r,right,id);
			return d.getValue(r,id);
		}
		ClassProto* getReturnType(RData& r) const;
		DATA& operator= (const DATA& d) {
			assert(d.type<6);
			type = d.type;
			data.pointer = d.data.pointer; info.pointer = d.info.pointer;
			return *this;
		}
		void* getInfo() const{
			return info.pointer;
		}
		DATA& operator= (DATA& d) {
			assert(d.type<6);
			type = d.type;
			data.pointer = d.data.pointer; info.pointer = d.info.pointer;
			return *this;
		}
		FunctionProto* getFunctionType() const{
			if(type!=R_FUNC && type!=R_GEN) PositionID(0,0,"<start.gft>").error("Could not gtf "+str<DataType>(type));
			assert(info.funcType !=NULL);
			return info.funcType;
		}
		static DATA getFunction(Function* f, FunctionProto* t){
			return DATA(R_FUNC, f, t);
		};
		static DATA getConstant(Value* v, ClassProto* c){
			return DATA(R_CONST, v, c);
		};
		static DATA getClass(ClassProto* c);
		static DATA getGenerator(E_GEN* s, FunctionProto* f){
			return DATA(R_GEN, s, f);
		};
		static DATA getLocation(Location* v, ClassProto* c){
			return DATA(R_LOC, v, c);
		};
		static DATA getNull(){
			return DATA(0);
		}
		DATA toLocation(RData& m);
		DATA toValue(RData& m,PositionID id){
#ifndef NDEBUG
			if(!(type==R_CONST || type==R_LOC)){
				cerr << "Cannot toValue of non const/loc "<< type << endl << flush;
				assert(0);
				exit(1);
			}
#endif
			if(type==R_CONST){
				return *this;
			} else {
				return DATA::getConstant(getValue(m,id), getReturnType(m));
			}
		}
		DataType getType() const{
			return type;
		};
		Location* getMyLocation() const{
#ifdef NDEBUG
			if(type!=R_LOC){
				cerr << "Cannot getLocation of non-location "<< type << endl << flush;
				exit(1);
			}
#else
			assert(type==R_LOC);
#endif
			assert(data.pointer!=NULL);
			return data.location;
		}
		void* getPointer() const{
			return data.pointer;
		}
		E_GEN* getMyGenerator() const{
			if(type!=R_GEN){
				cerr << "Cannot getGenerator of non-gen " << type << endl << flush;
				exit(1);
			}
			assert(data.pointer==NULL);
			return data.generator;
		}
		Function* getMyFunction() const{
			if(type!=R_FUNC){
				cerr << "Cannot getFunction of non-function " << type << endl << flush;
				exit(1);
			}
			assert(data.pointer!=NULL);
			return data.function;
		}
		ClassProto* getMyClass(RData& r) const;
		Value* getValue(RData& r,PositionID id) const;
		void setValue(RData& r, Value* v);
};

struct Jumpable {
	public:
		String name;
		TJump toJump;
		BasicBlock* start;
		BasicBlock* end;
		ClassProto* returnType;
		std::vector<std::pair<BasicBlock*,BasicBlock*>> resumes;
		std::vector<std::pair<BasicBlock*,DATA> > endings;
		Jumpable(String n, TJump t, BasicBlock* s, BasicBlock* e, ClassProto* p):
			name(n), toJump(t), start(s), end(e), returnType(p){

		}
};

#define RDATA_C_
struct RData{
		friend LazyLocation;
	private:
		std::vector<Jumpable*> jumps;
		std::map<Function*,std::vector<LazyLocation*> > flocs;
		//std::map<Function*,std::map<BasicBlock*,BasicBlock*> > pred;
	public:
		RData();
		bool guarenteedReturn;
		Module* lmod;
		FunctionPassManager* fpm;
		PassManager* mpm;
		IRBuilder<> builder;
		ExecutionEngine* exec;
		PHINode* CreatePHI(Type *Ty, unsigned NumReservedValues, const Twine &Name = ""){
			PHINode* p = builder.CreatePHI(Ty,NumReservedValues,Name);
			Instruction* s = &(builder.GetInsertBlock()->front());
			if(s!=p) p->moveBefore(s);
			return p;
		}
		void recursiveFinalize(LazyLocation* ll, std::map<BasicBlock*,std::pair<PHINode*,PositionID> >::iterator toFix);
		void addJump(Jumpable* j){
			jumps.push_back(j);
		}
		Jumpable* popJump(){
			auto a = jumps.back();
			jumps.pop_back();
			return a;
		}

		//createfunction endfunction
		Function* CreateFunctionD(String name,FunctionType* FT,Function::LinkageTypes L){
			Function* f = Function::Create(FT,L,name,lmod);
			return f;
		}
		Function* CreateFunction(String name,FunctionType* FT,Function::LinkageTypes L){
			Function* f = CreateFunctionD(name,FT,L);
			flocs.insert(std::pair<Function*,std::vector<LazyLocation*> >(f,std::vector<LazyLocation*>()));
			//pred.insert(std::pair<Function*,std::map<BasicBlock*,BasicBlock*> >(f,std::map<BasicBlock*,BasicBlock*>()));
			return f;
		}
		void FinalizeFunction(Function* f,bool debug=false);
		BasicBlock* CreateBlockD(String name,Function* F){
			BasicBlock* b = BasicBlock::Create(lmod->getContext(), name, F);
			return b;
		}
		BasicBlock* CreateBlock1(String name,Function* F){
			BasicBlock* b = BasicBlock::Create(lmod->getContext(), name, F);
			//auto found = pred.find(F);
			//assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
			//found->second.insert(std::pair<BasicBlock*,BasicBlock* >(b,NULL));
			return b;
		}
		BasicBlock* CreateBlock(String name, BasicBlock* p=NULL){
			Function* F = builder.GetInsertBlock()->getParent();
			BasicBlock* b = BasicBlock::Create(lmod->getContext(), name, F);
			//auto found = pred.find(F);
			//assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
			//found->second.insert(std::pair<BasicBlock*,BasicBlock* >(b,p));
			return b;
		}
		BasicBlock* getBlock(String name, JumpType jump, BasicBlock* bb, DATA val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume=std::pair<BasicBlock*,BasicBlock*>(NULL,NULL));
};
class StandardLocation : public Location{
	private:
		Value* position;
	public:
		StandardLocation(Value* a):position(a){}
		Value* getValue(RData& r, PositionID id) override final{
			return r.builder.CreateLoad(position);
		}
		virtual void setValue(Value* v, RData& r) override final{
			r.builder.CreateStore(v, position);
		}
		virtual Value* getPointer(RData& r,PositionID id){
			return position;
		}
};
class LazyLocation : public Location{
		friend RData;
	private:
		bool used;
		std::map<BasicBlock*,Value* > data;
		std::map<BasicBlock*,std::pair<PHINode*,PositionID> > phi;
		Value* position;
		Type* type;
		//String name;
		//mutable bool preset = false;
	public:
		LazyLocation(RData& r,Value* p, BasicBlock* b=NULL,Value* d=NULL,bool u = false
		):data(),position(p){
			static int i=0;
			//name = "Lazy."+str<int>(i++)+".";
			used = u;
			assert(position);
#ifndef NDEBUG
			Type* t;
			if(PointerType* pt = dyn_cast<PointerType>(p->getType())){
				t = pt->getElementType();
			} else assert(0 && "Cannot use non-pointer type for LazyLocation");
			type = t;
#else
			type = ((PointerType*) p->getType())->getElementType();
#endif
			assert(type);
#ifndef NDEBUG
			if(d!=NULL)
			assert(d->getType()==type);
#endif
			if(b!=NULL) data[b] = d;
			//if(d!=NULL) d->setName(name);
			r.flocs.find(r.builder.GetInsertBlock()->getParent())->second.push_back(this);
		}
		Value* getPointer(RData& r,PositionID id) override final{
			BasicBlock* me = r.builder.GetInsertBlock();
			auto found = data.find(me);
			if(found==data.end()){
				//not there -- create and insert phi-node unusable
				//BasicBlock* prev = r.pred.find(me->getParent())->second.find(me)->second;
				Value* v=NULL;
				//if(prev==NULL){
				PHINode* n = r.CreatePHI(type, 1U/*,name*/);
				phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >(me,std::pair<PHINode*,PositionID>(n,id)));
				v = n;
				/*} else {
					auto found2 = data.find(prev);
					if(found2==data.end())
						id.error("Attempting to get value from variable that has not been set");
					if(found2->second==NULL){
						r.builder.SetInsertPoint(prev);
						data[prev] = v = r.builder.CreateLoad(position);
						r.builder.SetInsertPoint(me);
					} else v = found->second;
				}*/
				r.builder.CreateStore(v,position);
				data[me] = NULL;
			}else if(found->second!=NULL){
				//there and usable -- load current into memory
				r.builder.CreateStore(found->second,position);
				data[me] = NULL;
				//TODO check if just can change iterator
			}
			used = true;
			return position;
		}
	private:
		inline Value* getFastValue(RData& r, std::map<BasicBlock*,Value*>::iterator found,bool set=false){
			if(found->second==NULL){
				if(set) r.builder.SetInsertPoint(found->first);
				LoadInst* v = new LoadInst(position/*,name*/);
				if(Instruction* in = dyn_cast<Instruction>(position)){
					BasicBlock* b = in->getParent();
					if(b==found->first) v->insertAfter(in);
					else v->insertBefore(found->first->getFirstNonPHI());
				}
				else v->insertBefore(found->first->getFirstNonPHI());
				found->second = v;
				return v;
			} else return found->second;
		}
	public:
		Value* getValue(RData& r, PositionID id) override final{
			BasicBlock* me = r.builder.GetInsertBlock();
			auto found = data.find(me);
			if(found==data.end()){
				//not there -- create and insert phi-node unusable
				//BasicBlock* prev = r.pred.find(me->getParent())->second.find(me)->second;
				Value* v=NULL;
				//if(prev==NULL){
				PHINode* n = r.CreatePHI(type, 1U/*,name*/);
				phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >(me,std::pair<PHINode*,PositionID>(n,id)));
				v = n;
				/*} else {
					auto found2 = data.find(prev);
					if(found2==data.end())
						id.error("Attempting to get value from variable that has not been set");
					if(found2->second==NULL){
						r.builder.SetInsertPoint(prev);
						data[prev] = v = r.builder.CreateLoad(position);
						r.builder.SetInsertPoint(me);
					} else v = found->second;
				}*/
				data[me] = v;
				return v;
			}else return getFastValue(r,found,true);
		}
		void setValue(Value* v, RData& r) override final{
			assert(v);
			assert(v->getType()==type);
			//v->setName(name);
			BasicBlock* me = r.builder.GetInsertBlock();
			data[me] = v;
		}
};

void RData::recursiveFinalize(LazyLocation* ll, std::map<BasicBlock*,std::pair<PHINode*,PositionID> >::iterator toFix){
	assert(toFix->second.first);
	PHINode* p = toFix->second.first;
	if(BasicBlock* prev = toFix->first->getUniquePredecessor()){
		//cerr << "has unique! "<<endl;fflush(stderr);
		auto found = ll->data.find(prev);
		if(found!=ll->data.end()){
			//cerr << "has value! "<<endl;fflush(stderr);
			Value* v = ll->getFastValue(*this,found);
			p->replaceAllUsesWith(v);
			p->eraseFromParent();
			auto repdata = ll->data.find(toFix->first);
			if(repdata->second==p) repdata->second = v;
			toFix->second.first = NULL;
		} else{
			//cerr << "no value "<<endl;fflush(stderr);
			assert(ll->phi.find(prev)==ll->phi.end());
			std::vector<BasicBlock*> cache(1,toFix->first);
			BasicBlock* tmp=prev;
			do{
				auto found3 = ll->data.find(tmp);
				if(found3!=ll->data.end() && dyn_cast_or_null<PHINode>(found3->second)==NULL){
					//cerr << "eventual unique value "<<endl;fflush(stderr);
					Value* v = ll->getFastValue(*this,found3);
					for(BasicBlock* bl:cache){
						std::map<BasicBlock*,std::pair<PHINode*,PositionID> >::iterator fin = ll->phi.find(bl);
						if(fin!=ll->phi.end() && fin->second.first!=NULL){
							PHINode* f = fin->second.first;
							assert(v->getType()==f->getType());
							f->replaceAllUsesWith(v);
							f->eraseFromParent();
							fin->second.first=NULL;
							if(ll->data[bl]==f) ll->data[bl] = v;
						}
					}
					return;
				}
				cache.push_back(tmp);
				//cerr << "skipped " << tmp->getName().str() << endl << flush;
				prev = tmp;
				tmp = prev->getUniquePredecessor();
			}while(tmp!=NULL);
			cache.pop_back();
			builder.SetInsertPoint(prev);
			auto found3 = ll->data.find(prev);
			//cerr << "no unique! "<<endl;fflush(stderr);
			if(found3!=ll->data.end()){
				//cerr << "no unique, but value! "<<endl;fflush(stderr);
				builder.SetInsertPoint(prev);
				Value* v = ll->getFastValue(*this,found3);
				for(BasicBlock* bl: cache){
					auto fin = ll->phi.find(bl);
					if(fin!=ll->phi.end() && fin->second.first!=NULL){
						PHINode* f = fin->second.first;
						assert(v->getType()==f->getType());
						f->replaceAllUsesWith(v);
						f->eraseFromParent();
						fin->second.first=NULL;
						if(ll->data[bl]==f) ll->data[bl]=v;
					}
					//	cerr << "finished with " << blocks[i]->getName().str() << endl << flush;
				}
				auto fina = ll->phi.find(prev);
				if(fina!=ll->phi.end() && fina->second.first!=NULL){
					//cerr << "reviewing " << prev->getName().str() << endl << flush;
					recursiveFinalize(ll,fina);
					fina->second.first=NULL;
				}
				return;
			}

			//cerr << "no unique, no value!"<<endl;fflush(stderr);
			builder.SetInsertPoint(prev);
			PHINode* np = CreatePHI(p->getType(), 1U/*,ll->name*/);
			ll->data[prev] = np;
			auto toRet = ll->phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >
			(prev, std::pair<PHINode*,PositionID>(np,toFix->second.second)));
			for(BasicBlock* bl:cache){
				auto fin = ll->phi.find(bl);
				if(fin!=ll->phi.end() && fin->second.first!=NULL){
					PHINode* f = fin->second.first;
					assert(np->getType()==f->getType());
					f->replaceAllUsesWith(np);
					f->eraseFromParent();
					fin->second.first=NULL;
					if(ll->data[bl]==f) ll->data[bl]=np;
				}
			}
			if(toRet.first->second.first!=NULL){
				recursiveFinalize(ll,toRet.first);
				toRet.first->second.first = NULL;
			}
		}
	} else{
		pred_iterator PI = pred_begin(toFix->first);
		pred_iterator E = pred_end(toFix->first);
		if(PI==E){
/*			toFix->first->getParent()->dump();
			cerr << endl << flush;
			toFix->first->dump();
			cerr << endl << flush;
			toFix->second.second.error("Variable was not defined here");
			*/
			Value* tmp = UndefValue::get(p->getType());
			p->replaceAllUsesWith(tmp);
			p->eraseFromParent();
			if(ll->data[toFix->first]==p) ll->data[toFix->first]=tmp;
			toFix->second.first=NULL;
			return;
		}
		toFix->second.first = NULL;
		do{
			BasicBlock* bb = *PI;
			auto found = ll->data.find(bb);
			if(found!=ll->data.end()){
				p->addIncoming(ll->getFastValue(*this,found),bb);
			} else{
				assert(ll->phi.find(bb)==ll->phi.end());
				builder.SetInsertPoint(bb);
				PHINode* np = CreatePHI(p->getType(), 1U/*,ll->name*/);
				auto toRet = ll->phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >
				(bb, std::pair<PHINode*,PositionID>(np,toFix->second.second)));
				ll->data[bb] = np;//todo speed up
				assert(np);
				assert(np->getType()==ll->type);
				p->addIncoming(np,bb);
				if(toRet.first->second.first!=NULL){
					recursiveFinalize(ll,toRet.first);
				}
			}
			++PI;
		}while(PI!=E);
	}
}
void RData::FinalizeFunction(Function* f,bool debug){
	BasicBlock* Parent = builder.GetInsertBlock();
	for(LazyLocation*& ll: flocs.find(f)->second){
		//		ll->phi.
		for(std::map<BasicBlock*,std::pair<PHINode*,PositionID> >::iterator it = ll->phi.begin(); it!=ll->phi.end(); ++it){
			if(it->second.first) recursiveFinalize(ll,it);
		}
		if(!ll->used){
			if(Instruction* u = dyn_cast<Instruction>(ll->position)) u->eraseFromParent();
		}
		delete ll;
	}
	if(Parent) builder.SetInsertPoint(Parent);
	//cerr << "start finalizing function" << endl << flush;
	if(debug){
		f->dump();
		cerr << endl << flush;
	}
	fpm->run(*f);
	//f->dump();
	//cerr << endl << flush;
	//cerr << "done finalizing function" << endl << flush;
}
bool isalpha(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z');
}
bool isalnum(char c){
	return (c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9');
}
#endif /* SETTINGS_HPP_ */
