/*
 * settings.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

void printi(uint64_t i, bool b){
	std::cout << i;
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
void printd(double i, bool b){
	std::cout << i;
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
void printb(bool i, bool b){
	if(i) std::cout << "true";
	else std::cout << "false";
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
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
using namespace llvm;
#include "../O_TOKEN.hpp"

#define cout std::cout
#define cin std::cin
#define cerr std::cerr
#define flush std::flush
#define endl std::endl << flush
#define String std::string
#define ostream std::ostream
//#define char int
#define byte unsigned short
#define exception std::exception
//#include "indexed_map.hpp"

struct PositionID{
	public:
		String fileName;
		unsigned int lineN;
		unsigned int charN;
		PositionID(unsigned int a, unsigned int b, String c){
			lineN = a;
			charN = b;
			fileName = c;
		}
		ostream& operator << (ostream& o){
			o << fileName;
			o << " line:";
			o << lineN;
			o << ", char: ";
			o << charN;
			return o;
		}
};

void todo(String a,PositionID filePos){
	cerr << a << " at ";
	cerr << filePos.fileName;
	cerr << " line:";
	cerr << filePos.lineN;
	cerr << ", char: ";
	cerr << filePos.charN;
	cerr << endl << flush;
	exit(1);
}

#ifndef DECLR_P_
#define DECLR_P_
class Declaration;
#endif

class OModule;
#ifndef OOBJECT_P_
#define OOBJECT_P_
class oobject;
#endif


enum JumpType{
	RETURN = 0,
	BREAK = 1,
	CONTINUE = 2
};

enum TJump{
	FUNC = 0,
	LOOP = 1
};


class ClassProto;

struct Jumpable {
	public:
		String name;
		TJump toJump;
		BasicBlock* start;
		BasicBlock* end;
		ClassProto* returnType;
		std::vector<std::pair<BasicBlock*,Value*> > endings;
		Jumpable(String n, TJump t, BasicBlock* s, BasicBlock* e, ClassProto* p):
			name(n), toJump(t), start(s), end(e), returnType(p){
		}
};
struct RData{
	private:
		std::vector<Jumpable*> jumps;
	public:
		RData();
		bool guarenteedReturn;
		Module* lmod;
		FunctionPassManager* fpm;
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
		BasicBlock* getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, RData& rd);
};

//#ifndef OCLASS_P_
//#define OCLASS_P_
//class ClassProto;
//#endif

class obinop;


class ouop{
	public:
		virtual Value* apply(Value* a, RData& mod) = 0;
		ClassProto* returnType;
};

class ouopNative: public ouop{
	public:
		Value* (*temp)(Value*,RData&);
		ouopNative(Value* (*fun)(Value*,RData&),ClassProto* a){
			temp = fun;
			returnType = a;
		}
		Value* apply(Value* a,RData& m){
			return temp(a,m);
		}
};

class FunctionProto;

class ClassProto{
	private:
		std::map<String,std::map<ClassProto*, obinop*> > binops;
		std::map<ClassProto*, ouop*> casts;
		//		std::map<String,std::map<ClassProto*, obinop*> > binops;
	public:
		ClassProto* superClass;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		Type* type;
		String name;
		bool hasCast(ClassProto* right){
			auto found = casts.find(right);
			return found!=casts.end();
		}
		ouop*& addCast(ClassProto* right){
			if(hasCast(right)) todo("Error: Redefining cast "+name+" to "+right->name,PositionID(0,0,"<start>"));
			return casts[right];
		}
		Value* castTo(RData& r, Value* c, ClassProto* right){
			if(hasCast(right)) return casts[right]->apply(c, r);
			else{
				todo("Compile error - could not find cast "+name+" to "+right->name,PositionID(0,0,"<start>"));
				return NULL;
			}
		}
		obinop*& addBinop(String operation, ClassProto* right){
			auto found = binops.find(operation);
			if(found!=binops.end()){
				auto found2 = found->second.find(right);
				if(found2!=found->second.end()){
					todo("Error: Redefining binary operation '"+operation+"' from "+name+" to "+right->name,
							PositionID(0,0,"<start>"));
				}
			}
			return binops[operation][right];
		}
		std::pair<obinop*, std::pair<ouop*,ouop*> > getBinop(PositionID id, String operation, ClassProto* right){
			ClassProto* self = this;
			while(self!=NULL){
				auto found = self->binops.find(operation);
				if(found==self->binops.end()){
					self = self->superClass;
					continue;
				}
				auto thisToSelf = this->casts.find(self);
				if(thisToSelf==this->casts.end()){
					self = self->superClass;
					continue;
				}
				auto look = found->second;
				auto toCheck = right;
				while(toCheck!=NULL){
					auto found2 = look.find(toCheck);
					if(found2==look.end()){
						toCheck = toCheck->superClass;
						continue;
					}
					auto found3 = right->casts.find(toCheck);
					if(found3==right->casts.end()){
						toCheck = toCheck->superClass;
						continue;
					}
					return std::pair<obinop*,std::pair<ouop*,ouop*> >(found2->second,
							std::pair<ouop*,ouop*>(thisToSelf->second, found3->second)
					);
				}
				self = self->superClass;
			}
			todo("Binary operator "+operation+" not implemented for class "+
					name+ " with right "+ right->name, id);
			return std::pair<obinop*,std::pair<ouop*,ouop*> >(NULL,
					std::pair<ouop*,ouop*>(NULL,NULL));
		}
		ClassProto* leastCommonAncestor(ClassProto* c){
			std::set<ClassProto*> mySet;
			std::vector<ClassProto*> todo = {this, c};
			while(todo.size()>0){
				ClassProto* tmp = todo.back();
				auto tmp2 = mySet.find(tmp);
				if(tmp2!=mySet.end()) return *(tmp2);
				else{
					todo.pop_back();
					if(tmp->superClass!=NULL) todo.push_back(tmp->superClass);
					mySet.insert(tmp);
				}
			}
			return NULL;
		}
		ClassProto(ClassProto* sC, String n, Type* t=NULL) : superClass(sC), type(t), name(n) {
			auto temp = new ouopNative(
					[](Value* a, RData& m) -> Value*{
				return a;
			}
			, this);
			casts.insert(std::pair<ClassProto*, ouop*>(this,temp));
		}
};


class FunctionProto{
	public:
		String name;
		std::vector<Declaration*> declarations;
		ClassProto* returnType;
		FunctionProto(String n, std::vector<Declaration*>& a, ClassProto* r):name(n),declarations(a), returnType(r){}
		FunctionProto(String n, ClassProto* r=NULL):name(n),declarations(), returnType(r){}
};

class obinop{
	public:
		virtual Value* apply(Value* a, Value* b, RData& mod) = 0;
		ClassProto* returnType;
};

class obinopNative : public obinop{
	public:
		Value* (*temp)(Value*,Value*,RData&);
		obinopNative(Value* (*fun)(Value*,Value*,RData&), ClassProto* a){
			temp = fun;
			returnType = a;
		}
		Value* apply(Value* a, Value* b, RData& m){
			return temp(a,b,m);
		}
};

auto VOIDTYPE = Type::getVoidTy (getGlobalContext());
auto BOOLTYPE = IntegerType::get(getGlobalContext(), 1);
auto INTTYPE = IntegerType::get(getGlobalContext(), 64);
auto DOUBLETYPE = Type::getDoubleTy(getGlobalContext());
auto CHARTYPE = IntegerType::get(getGlobalContext(), 8);
std::vector<Type*> __str_struct = {INTTYPE, PointerType::get(CHARTYPE, 0)};
auto STRINGTYPE = StructType::create(__str_struct);

ArrayRef<int> getArrayRefFromString(String s){
	std::vector<int> v;
	for(auto a:s) v.push_back(a);
	return ArrayRef<int>(v);
}

/*
auto& getLLVMString(String s){
	//auto tmp = getArrayRefFromString(s);
	//auto dat = ConstantDataArray::get(getGlobalContext(), tmp);

}*/

//PointerType::getUnqual(ConstantDataArray::getString(getGlobalContext(),"")->getType());
//		ConstantDataArray::getString(getGlobalContext(),"")->getType();

ClassProto* autoClass = new ClassProto(NULL, "auto");
ClassProto* objectClass = new ClassProto(NULL, "object");
ClassProto* classClass = new ClassProto(objectClass, "class");
//ClassProto* autoClass = new ClassProto("auto"); todo auto class
//ClassProto* nullClass = new ClassProto("None");
ClassProto* boolClass = new ClassProto(objectClass, "bool", BOOLTYPE);
ClassProto* arrayClass = new ClassProto(objectClass, "array");
ClassProto* functionClass = new ClassProto(objectClass, "function");
ClassProto* decClass = new ClassProto(objectClass, "double", DOUBLETYPE);
ClassProto* intClass = new ClassProto(decClass, "int", INTTYPE);
ClassProto* stringClass = new ClassProto(objectClass, "string",STRINGTYPE);
ClassProto* sliceClass = new ClassProto(objectClass, "slice");
ClassProto* voidClass = new ClassProto(objectClass, "void", VOIDTYPE); //todo check?

#include "operators.hpp"
template<typename C> bool in(const std::vector<C> a, C b){
	for(const auto& e: a)
		if(e==b) return true;
	return false;
}


BasicBlock* RData::getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, RData& rd){
	if(name==""){
		if(jump==RETURN){
			for(unsigned int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == FUNC){
					if(ret==NULL || !ret->hasCast(jumps[i]->returnType)){
						cerr << "Invalid return type, trying to use a " << ((ret==NULL)?"null":(ret->name)) << " instead of a " << jumps[i]->returnType->name << endl << flush;
						exit(0);
					}
					jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,ret->castTo(rd, val, jumps[i]->returnType)));
					return jumps[i]->end;
				}
				if(i == 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(unsigned int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i == 0){
					cerr << "Error could not find continue/break block" << endl << flush;
					return NULL;
				}
			}
		}
	} else {
		cerr << "Error not done yet2" << endl << flush;
		exit(1);
		return NULL;
	}
}
#endif /* SETTINGS_HPP_ */
