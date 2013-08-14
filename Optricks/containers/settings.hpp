/*
 * settings.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_
#include <llvm/Analysis/Verifier.h>
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
using namespace llvm;
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
//#include "../O_TOKEN.hpp"

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

class OModule;
class oclass;
struct RData{
	public:
		RData();
		OModule* module;
		IRBuilder<> builder;
};

class obinop{
	public:
		virtual Value* apply(Value* a, Value* b, RData& mod) = 0;
		oclass* returnType;
};

class obinopNative : public obinop{
	public:
		Value* (*temp)(Value*,Value*,RData&);
		obinopNative(Value* (*fun)(Value*,Value*,RData&), oclass* a){
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
		oclass* returnType;
};

class ouopNative: public ouop{
	public:
		Value* (*temp)(Value*,RData&);
		ouopNative(Value* (*fun)(Value*,RData&),oclass* a){
			temp = fun;
			returnType = a;
		}
		Value* apply(Value* a,RData& m){
			return temp(a,m);
		}
};

void todo(String a="",String b="",String c="",String d="",String e="",String f="",String g=""){
	cerr << a << b << c << d << e << f<<g<<endl << flush;
	exit(1);
}

IntegerType* llvmChar(RData& a,LLVMContext& context){
	return IntegerType::get(context, 8);
}
#endif /* SETTINGS_HPP_ */
