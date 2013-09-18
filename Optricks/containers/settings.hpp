/*
 * settings.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include "basic_functions.hpp"

#include <stdio.h>
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
#include <unordered_set>
#include "basic_functions.hpp"

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

#define DATA Value*

#include "../O_TOKEN.hpp"
#include "types.hpp"
//#include "indexed_map.hpp"

struct ComplexStruct{
		double real, complex;
};
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
		PositionID():PositionID(0,0,"<start>"){}
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

ArrayRef<int> getArrayRefFromString(String s){
	std::vector<int> v;
	for(auto a:s) v.push_back(a);
	return ArrayRef<int>(v);
}


#ifndef OPERATIONS_C_
#define OPERATIONS_C_
class obinop;
class ouop;
class obinopNative;
class ouopNative;
#endif
#ifndef STATEMENT_P_
#define STATEMENT_P_
class Statement;
#endif

#ifndef CLASSPROTO_C_
#define CLASSPROTO_C_
class ClassProto;
#endif

#ifndef DECLR_P_
#define DECLR_P_
class Declaration;
#endif


/*
auto& getLLVMString(String s){
	//auto tmp = getArrayRefFromString(s);
	//auto dat = ConstantDataArray::get(getGlobalContext(), tmp);

}*/

//PointerType::getUnqual(ConstantDataArray::getString(getGlobalContext(),"")->getType());
//		ConstantDataArray::getString(getGlobalContext(),"")->getType();



#endif /* SETTINGS_HPP_ */
