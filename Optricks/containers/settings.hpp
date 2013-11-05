/*
 * settings.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "basic_functions.h"

#include <GL/glut.h>
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

template<> String str<DataType>(DataType d){
	switch(d){
		case R_CONST: return "R_CONST";
		case R_CLASS: return "R_CLASS";
		case R_FUNC: return "R_FUNC";
		case R_GEN: return "R_GEN";
		case R_LOC: return "R_LOC";
		case R_UNDEF: return "R_UNDEF";
	}
	return "unknown DATATYPE";
}
union PrivateData{
	Value* constant;
	ClassProto* classP;
	Function* function;
	E_GEN* generator;
	Value* location;
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

class PositionID{
	public:
		String fileName;
		unsigned int lineN;
		unsigned int charN;
		PositionID(unsigned int a, unsigned int b, String c){
			lineN = a;
			charN = b;
			fileName = c;
		}
	//	PositionID():PositionID(0,0,"<start>"){}
		void error(String s, bool end=true) const{
			cerr << s << " at " << fileName << " on line " << lineN << ", char " << charN << endl << flush;
			if(end){
			#ifdef NDEBUG
			exit(1);
#else
			assert(0);
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
		static DATA getLocation(Value* v, ClassProto* c){
			return DATA(R_LOC, v, c);
		};
		static DATA getNull(){
			return DATA(0);
		}
		DATA toLocation(RData& m);
		DATA toValue(RData& m){
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
				return DATA::getConstant(getValue(m), getReturnType(m));
			}
		}
		DataType getType() const{
			return type;
		};
		Value* getMyLocation() const{
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
		Value* getValue(RData& r) const;
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
	private:
		std::vector<Jumpable*> jumps;
	public:
		RData();
		bool guarenteedReturn;
		Module* lmod;
		FunctionPassManager* fpm;
		PassManager* mpm;
		IRBuilder<> builder;
		ExecutionEngine* exec;
		void addJump(Jumpable* j){
			jumps.push_back(j);
		}
		Jumpable* popJump(){
			auto a = jumps.back();
			jumps.pop_back();
			return a;
		}
		BasicBlock* getBlock(String name, JumpType jump, BasicBlock* bb, DATA val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume=std::pair<BasicBlock*,BasicBlock*>(NULL,NULL));
};
#endif /* SETTINGS_HPP_ */
