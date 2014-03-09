/*
 * includes.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#define  LOCAL_FUNC Function::PrivateLinkage
#define EXTERN_FUNC Function::ExternalLinkage
#define VERIFY(A)
//#define VERIFY(A) verifyFunction(A);
//#include <GL/glut.h>
#undef VOID
#define __cplusplus 201103L
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <list>
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

#include <unistd.h>

#include <limits.h> /* PATH_MAX */
#include <gmp.h>
#include <gmpxx.h>
#include <mpfr.h>
#include <mpf2mpfr.h>

#include "llvm/ADT/APSInt.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Analysis/Passes.h"
//#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
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

#ifndef REFERENCECLASS_C_
#define REFERENCECLASS_C_
class ReferenceClass;
#endif

#ifndef LAZYCLASS_C_
#define LAZYCLASS_C_
class LazyClass;
#endif

#ifndef ABSTRACTCLASS_C_
#define ABSTRACTCLASS_C_
class AbstractClass;
#endif

#ifndef LEXER_C_
#define LEXER_C_
class Lexer;
#endif

#ifndef INTCLASS_C_
#define INTCLASS_C_
class IntClass;
#endif

#ifndef FLOATCLASS_C_
#define FLOATCLASS_C_
class FloatClass;
#endif

#ifndef FUNCCLASS_C_
#define FUNCCLASS_C_
class FunctionClass;
#endif

#ifndef COMPLEXCLASS_C_
#define COMPLEXCLASS_C_
class ComplexClass;
#endif

#ifndef DATA_C_
#define DATA_C_
class Data;
#endif

#ifndef CONSTANTDATA_C_
#define CONSTANTDATA_C_
class ConstantData;
#endif

#ifndef LOCATIONDATA_C_
#define LOCATIONDATA_C_
class LocationData;
#endif

#ifndef EVALUATABLE_C_
#define EVALUATABLE_C_
class Evaluatable;
#endif

#ifndef RDATA_C_
#define RDATA_C_
class RData;
#endif

#ifndef SINGLEFUNC_C_
#define SINGLEFUNC_C_
class SingleFunction;
#endif

#ifndef OVERLOADEDFUNC_C_
#define OVERLOADEDFUNC_C_
class OverloadedFunction;
#endif

#ifndef FUNCTIONPROTO_C_
#define FUNCTIONPROTO_C_
class FunctionProto;
#endif

#ifndef E_GEN_C_
#define E_GEN_C_
class E_GEN;
#endif

#ifndef LAZY_LOCATION_C_
#define LAZY_LOCATION_C_
class LazyLocation;
#endif

#ifndef E_FUNC_CALL_C_
#define E_FUNC_CALL_C_
class E_FUNC_CALL;
#endif

inline ConstantInt* getInt32(int32_t val){
	return ConstantInt::getSigned(IntegerType::get(getGlobalContext(),32),(int64_t)val);
}

const auto C_POINTERTYPE = PointerType::get(IntegerType::get(getGlobalContext(), 8),0);
const auto VOIDTYPE = Type::getVoidTy(getGlobalContext());
const auto BOOLTYPE = IntegerType::get(getGlobalContext(),1);
const auto CLASSTYPE = IntegerType::get(getGlobalContext(),8*sizeof(void*));
#ifdef NULL
#undef NULL
#endif
#define NULL nullptr

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

template<typename C> inline String str(C a){
	std::stringstream ss;
	ss << a;
	return ss.str();
}

enum LayoutType {
	LITERAL_LAYOUT = 3,
	PRIMITIVE_LAYOUT = 2,
	PRIMITIVEPOINTER_LAYOUT = 1,
	POINTER_LAYOUT = 0
};
template<> String str<LayoutType>(LayoutType d){
	switch(d){
	case LITERAL_LAYOUT: return "LITERAL_LAYOUT";
		case PRIMITIVE_LAYOUT: return "PRIMITIVE_LAYOUT";
		case PRIMITIVEPOINTER_LAYOUT: return "PRIMITIVEPOINTER_LAYOUT";
		case POINTER_LAYOUT: return "POINTER_LAYOUT";
	}
	return "unknown LayoutType";
}

enum ClassType{
	CLASS_AUTO,
	CLASS_FUNC,
	CLASS_CPOINTER,
	CLASS_GEN,
	CLASS_BOOL,
	CLASS_FLOAT, // floating point classes and literals
	CLASS_NULL,
	CLASS_MATHLITERAL,
	CLASS_INT, // int classes and literals
		CLASS_INTLITERAL,
	CLASS_STR, // string classes and literals
	CLASS_CHAR, // char classes and literals
	CLASS_COMPLEX,
	CLASS_RATIONAL,
	CLASS_TUPLE,
	CLASS_NAMED_TUPLE,
	CLASS_ARRAY,
	CLASS_MAP,
	CLASS_SET,
	CLASS_VECTOR,
	CLASS_VOID,
	CLASS_REF,
//	CLASS_FILE,
	CLASS_CLASS,
	CLASS_LAZY,
	CLASS_USER,
	CLASS_FLOATLITERAL
};

enum DataType{
	R_VOID,
	R_CONST,
	R_BOOL,
	R_LOC,
	R_FUNCMAP,
	R_FUNC,
	R_GEN,
	R_CLASS,
	R_CHAR,
	R_STR,
	R_INT,
	R_RATIONAL,
	R_FLOAT,
	R_FILE,
	R_IMAG,
	R_EXACT,
	R_TUPLE,
	R_ARRAY,
	R_SET,
	R_MAP,
	R_SLICE,
	R_MATH,
	R_CLASSFUNC,
	R_NULL
};
template<> String str<DataType>(DataType d){
	switch(d){
	case R_VOID: return "R_VOID";
	case R_MATH: return "R_MATH";
	case R_BOOL: return "R_BOOL";
	case R_SLICE: return "R_SLICE";
	case R_CONST: return "R_CONST"; // constant variable
	case R_LOC: return "R_LOC";
	case R_FUNCMAP: return "R_FUNCMAP";
	case R_FUNC: return "R_FUNC";
	case R_GEN: return "R_GEN";
	case R_CLASS: return "R_CLASS";
	case R_CHAR: return "R_CHAR";
	case R_STR: return "R_STR";
	case R_INT: return "R_INT";
	case R_RATIONAL: return "R_RATIONAL";
	case R_FLOAT: return "R_FLOAT";
	case R_FILE: return "R_FILE";
	case R_IMAG: return "R_IMAG";
	case R_EXACT: return "R_EXACT";
	case R_TUPLE: return "R_TUPLE";
	case R_ARRAY: return "R_ARRAY";
	case R_SET: return "R_SET";
	case R_MAP: return "R_MAP";
	case R_NULL: return "R_NULL";
	case R_CLASSFUNC: return "R_CLASSFUNC";
	}
	return "unknown DATATYPE";
}
enum MathConstant{
	MATH_PI,
	MATH_EULER_MASC,
	MATH_E,
	MATH_LN2,
	MATH_CATALAN,
};
template<typename N> inline N check(const N& a){
	assert(a);
	return a;
}
#include "lib.hpp"
#include "output.hpp"
#endif /* INCLUDES_HPP_ */
