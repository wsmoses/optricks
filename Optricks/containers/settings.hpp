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
#include <sstream>
#include <exception>
#include <stdexcept>
#include <unordered_map>

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

#ifndef DECLR_P_
#define DECLR_P_
class Declaration;
#endif

class OModule;
#ifndef OOBJECT_P_
#define OOBJECT_P_
class oobject;
#endif

//#ifndef OCLASS_P_
//#define OCLASS_P_
//class ClassProto;
//#endif

class obinop;
class ouop;

struct ClassProto{
	public:
		std::map<String,std::map<ClassProto*, obinop*> > binops;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		Type* type;
		String name;
		ClassProto(String n, Type* t=NULL) : type(t), name(n) {}
};


struct FunctionProto{
	public:
		String name;
		std::vector<Declaration*> declarations;
		ClassProto* returnType;
		FunctionProto(String n, std::vector<Declaration*>& a, ClassProto* r):name(n),declarations(a), returnType(r){}
		FunctionProto(String n, ClassProto* r=NULL):name(n),declarations(), returnType(r){}
};

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


enum JumpType{
	RETURN = 0,
	BREAK = 1,
	CONTINUE = 2
};

enum TJump{
	FUNC = 0,
	LOOP = 1
};

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
//		OModule* module;
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
		BasicBlock* getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val){
			if(name==""){
				if(jump==RETURN){
					for(unsigned int i = jumps.size()-1; ; i--){
						if(jumps[i]->toJump == FUNC){
							if(jumps[i]->returnType != ret){
								cerr << "Invalid return type, trying to use a " << ret->name << " instead of a " << jumps[i]->returnType->name << endl << flush;
								exit(0);
							}
							jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
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

ClassProto* classClass = new ClassProto("class");
ClassProto* objectClass = new ClassProto("object");
//ClassProto* autoClass = new ClassProto("auto"); todo auto class
ClassProto* nullClass = new ClassProto("None");
ClassProto* boolClass = new ClassProto("bool", BOOLTYPE);
ClassProto* arrayClass = new ClassProto("array");
ClassProto* functionClass = new ClassProto("function");
ClassProto* decClass = new ClassProto("double", DOUBLETYPE);
ClassProto* intClass = new ClassProto("int", INTTYPE);
ClassProto* stringClass = new ClassProto("string",STRINGTYPE);
ClassProto* sliceClass = new ClassProto("slice");
ClassProto* voidClass = new ClassProto("void", VOIDTYPE);

bool isStartName(int i){
	return isalpha(i) || i=='_' || i=='$';
}
void initClassesMeta(){
	///////******************************* Boolean ********************************////////
	/* //TODO short-circuit
	boolClass->binops["&&"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->binops["||"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);
	*/
	boolClass->binops["&"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->binops["|"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);

	boolClass->binops["^"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateXor(a,b,"xortmp");
	},boolClass);

	boolClass->binops["!="][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpNE(a,b,"andtmp");
	},boolClass);

	boolClass->binops["=="][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpEQ(a,b,"andtmp");
	},boolClass);

	boolClass->preops["!"] = new ouopNative(
			[](Value* a, RData& m) -> Value*{
				return m.builder.CreateNot(a,"nottmp");
	},boolClass);

	///////******************************* Double/Double ******************************////////
	decClass->binops["+"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(a,b,"addtmp");
	},decClass);

	decClass->binops["-"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(a,b,"subtmp");
	},decClass);

	decClass->binops["*"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(a,b,"multmp");
	},decClass);

	decClass->binops["%"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(a,b,"modtmp");
	},decClass);

	decClass->binops["<"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops[">"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["<="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops[">="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["=="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["!="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["/"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(a,b,"divtmp");
	},decClass);

	decClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateFNeg(a,"negtmp");
	},decClass);

	decClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},decClass);


	///////******************************* INT ********************************////////
	intClass->binops["&"][intClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	intClass->binops["|"][intClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},intClass);

	intClass->binops["^"][intClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateXor(a,b,"xortmp");
	},intClass);

	intClass->binops["+"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateAdd(a,b,"addtmp");
	},intClass);

	intClass->binops["-"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSub(a,b,"subtmp");
	},intClass);

	intClass->binops["*"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateMul(a,b,"multmp");
	},intClass);

	intClass->binops["%"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSRem(a,b,"modtmp");
	},intClass);

	intClass->binops["<"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["<="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["=="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["!="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["/"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSDiv(a,b,"divtmp");
	},intClass);

	intClass->binops["<<"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateShl(a,b);
	},intClass);

	intClass->binops[">>"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateAShr(a,b);
	},intClass);

	intClass->binops[">>>"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateLShr(a,b);
	},intClass);

	intClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateNeg(a,"negtmp");
	},intClass);

	intClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},intClass);

	intClass->preops["~"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
				return m.builder.CreateNot(a,"negtmp");
	},intClass);

	///////******************************* INT/Double ********************************////////
	intClass->binops["+"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(m.builder.CreateSIToFP(a,b->getType()),b,"addtmp");
	},decClass);

	intClass->binops["-"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(m.builder.CreateSIToFP(a,b->getType()),b,"subtmp");
	},decClass);

	intClass->binops["*"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(m.builder.CreateSIToFP(a,b->getType()),b,"multmp");
	},decClass);

	intClass->binops["%"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(m.builder.CreateSIToFP(a,b->getType()),b,"modtmp");
	},decClass);

	intClass->binops["<"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["<="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["=="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["!="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["/"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(m.builder.CreateSIToFP(a,b->getType()),b,"divtmp");
	},decClass);

	///////******************************* DOUBLE/int ********************************////////
		decClass->binops["+"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFAdd(a,m.builder.CreateSIToFP(b,a->getType()),"addtmp");
		},decClass);

		decClass->binops["-"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFSub(a,m.builder.CreateSIToFP(b,a->getType()),"subtmp");
		},decClass);

		decClass->binops["*"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFMul(a,m.builder.CreateSIToFP(b,a->getType()),"multmp");
		},decClass);

		decClass->binops["%"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFRem(a,m.builder.CreateSIToFP(b,a->getType()),"modtmp");
		},decClass);

		decClass->binops["<"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpULT(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops[">"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUGT(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["<="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpULE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops[">="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUGE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["=="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUEQ(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["!="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUNE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["/"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFDiv(a,m.builder.CreateSIToFP(b,a->getType()),"divtmp");
		},decClass);
	/*
	LANG_M->addPointer("class",classClass,0);
	LANG_M->addPointer("object",objectClass,0);
	LANG_M->addPointer("bool",boolClass,0);
	LANG_M->addPointer("array",arrayClass,0);
	LANG_M->addPointer("function",functionClass,0);
	LANG_M->addPointer("int",intClass,0);
	LANG_M->addPointer("double",decClass,0);
	LANG_M->addPointer("string",stringClass,0);
	LANG_M->addPointer("slice",sliceClass,0);*/
}

template<typename C> bool in(const std::vector<C> a, C b){
	for(const auto& e: a)
		if(e==b) return true;
	return false;
}

#endif /* SETTINGS_HPP_ */
