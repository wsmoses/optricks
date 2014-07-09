/*
 * includes.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef INCLUDES_HPP_
#define INCLUDES_HPP_

#define  LOCAL_FUNC llvm::Function::PrivateLinkage
#define EXTERN_FUNC llvm::Function::ExternalLinkage
#define VERIFY(A)
//#define VERIFY(A) verifyFunction(A);
#ifdef USE_OPENGL
#include <GL/glut.h>
#endif
#undef VOID
#define __cplusplus 201103L

#if (__GNUC__ && __cplusplus && __GNUC__ >= 3)
#include <cxxabi.h>
#endif
#ifdef NDEBUG
#undef NDEBUG
#endif
#ifdef WITH_ASSERTS
#define NDEBUG
#endif
#include <unistd.h>
#include <cassert>
#include <dirent.h>
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <list>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath>
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
//#include <dlfcn.h>
#include <sys/stat.h>

#include <limits.h> /* PATH_MAX */
#include <gmp.h>
#include <gmpxx.h>
#include <mpfr.h>
#include <mpf2mpfr.h>

#ifdef USE_SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#endif

#include <llvm/IR/Intrinsics.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/InitializePasses.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#if defined(LLVM_VERSION_MAJOR) && LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR <= 4
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/CFG.h>
#else
#include <llvm/IR/Verifier.h>
#include <llvm/IR/CFG.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#include <mmsystem.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <err.h>
#endif

//#include "clang/AST/ASTContext.h"
//#include "clang/AST/CharUnits.h"
//#include "clang/AST/DeclCXX.h"
//#include "clang/AST/DeclObjC.h"
//#include "clang/AST/DeclTemplate.h"
//#include "clang/AST/Mangle.h"
//#include "clang/AST/RecordLayout.h"
//#include "clang/AST/RecursiveASTVisitor.h"
//#include "clang/Basic/Builtins.h"
//#include "clang/Basic/CharInfo.h"
//#include "clang/Basic/Diagnostic.h"
//#include "clang/Basic/Module.h"
//#include "clang/Basic/SourceManager.h"
//#include "clang/Basic/TargetInfo.h"
//#include "clang/Basic/Version.h"
//#include "clang/Frontend/CodeGenOptions.h"
//#include "clang/Sema/SemaDiagnostic.h"
//#include "clang/AST/GlobalDecl.h"

#include "Macros.hpp"
//#define cout std::cout
#define cin std::cin
#define cerr std::cerr
#define flush std::flush
#define endl std::endl << flush
typedef std::string String;
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


#ifndef GEN_C_
#define GEN_C_
class GeneratorClass;
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

#ifndef SCOPABLE_C_
#define SCOPABLE_C_
class Scopable;
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

#ifndef STATEMENT_C_
#define STATEMENT_C_
class Statement;
#endif

template<typename T> inline void toStringStream(ostream& o, T s)
{
    o << s;
}

template<typename T, typename... Args> inline void toStringStream(ostream& o, T s, Args... args){
	o << s;
	toStringStream(o, args...);
}

template<typename... Args> String toStr(Args... args){
	std::stringstream ss;
	toStringStream(ss, args...);
	return ss.str();
}

const auto C_POINTERTYPE = llvm::PointerType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8),0);
const auto C_SIZETTYPE = llvm::IntegerType::get(llvm::getGlobalContext(),8*sizeof(size_t));
const auto C_INTTYPE = llvm::IntegerType::get(llvm::getGlobalContext(),8*sizeof(int));
const auto INT32TYPE = llvm::IntegerType::get(llvm::getGlobalContext(),32);
const auto VOIDTYPE = llvm::Type::getVoidTy(llvm::getGlobalContext());
const auto BOOLTYPE = llvm::IntegerType::get(llvm::getGlobalContext(),1);
const auto CHARTYPE = llvm::IntegerType::get(llvm::getGlobalContext(),8);
const auto CLASSTYPE = llvm::IntegerType::get(llvm::getGlobalContext(),8*sizeof(void*));
const auto C_STRINGTYPE = llvm::PointerType::getUnqual(CHARTYPE);

inline llvm::ConstantInt* getSizeT(size_t val){
	return llvm::ConstantInt::get(C_SIZETTYPE,(uint64_t)val,false);
}
inline llvm::ConstantInt* getInt32(int32_t val){
	return llvm::ConstantInt::getSigned(INT32TYPE,(int64_t)val);
}
inline llvm::ConstantInt* getCInt(int val){
	return llvm::ConstantInt::getSigned(INT32TYPE,(int64_t)val);
}
inline llvm::ConstantInt* getCUInt(unsigned int val){
	return llvm::ConstantInt::get(INT32TYPE,(uint64_t)val,false);
}

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
	CLASS_STRLITERAL,
	CLASS_CHAR, // char classes and literals
	CLASS_COMPLEX,
	CLASS_RATIONAL,
	CLASS_TUPLE,
	CLASS_NAMED_TUPLE,
	CLASS_PRIORITYQUEUE,
	CLASS_ARRAY,
	CLASS_MAP,
	CLASS_SET,
	CLASS_VECTOR,
	CLASS_VOID,
	CLASS_REF,
	CLASS_WRAPPER,
	CLASS_CSTRING,
	CLASS_HASHMAP,
//	CLASS_FILE,
	CLASS_CLASS,
	CLASS_SCOPE,
	CLASS_LAZY,
	CLASS_USER,
	CLASS_ENUM,
	CLASS_FLOATLITERAL
};

enum DataType{
	R_VOID,
	R_CONST,
	R_REF,
	R_DEC,
	R_LAZY,
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
	R_NULL,
	R_CLASSTEMPLATE
};
template<> String str<DataType>(DataType d){
	switch(d){
	case R_DEC: return "R_DEC";
	case R_CLASSTEMPLATE: return "R_CLASSTEMPLATE";
	case R_LAZY: return "R_LAZY";
	case R_VOID: return "R_VOID";
	case R_MATH: return "R_MATH";
	case R_REF: return "R_REF";
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

template<> String str<llvm::GlobalValue::LinkageTypes>(llvm::GlobalValue::LinkageTypes d){
	switch(d){
#define CASE(X) case llvm::GlobalValue::LinkageTypes::X: return #X;
CASE(ExternalLinkage)
CASE(AvailableExternallyLinkage)
CASE(LinkOnceAnyLinkage)
CASE(LinkOnceODRLinkage)
CASE(WeakAnyLinkage)
CASE(WeakODRLinkage)
CASE(AppendingLinkage)
CASE(InternalLinkage)
CASE(PrivateLinkage)
CASE(ExternalWeakLinkage)
CASE(CommonLinkage)
#undef CASE
	}
	return "unknown LinkageType";
}
enum MathConstant{
	MATH_PI,
	MATH_EULER_MASC,
	MATH_E,
	MATH_LN2,
	MATH_CATALAN
};
template<typename N> inline N check(const N& a){
	assert(a);
	return a;
}

enum TemplateArgType{
	TEMPLATE_CLASS,
	TEMPLATE_INT
};

/*
struct TemplateArg {
	//TemplateArgType argType;
	union TemplateValue{
		const AbstractClass* t_class;
	//	mpz_t t_int;
	} argValue;
	TemplateArg(const AbstractClass* a){
	//	argType = TEMPLATE_CLASS;
		argValue.t_class = a;
	}
	//TemplateArg(const mpz_t& v){
	//	argType = TEMPLATE_INT;
	//	mpz_init_set(argValue.t_int,v);
	//}
};
*/


#include "lib.hpp"

struct T_ARGS {
private:
	mutable std::vector<const AbstractClass*> evals;
public:
	bool inUse;
	bool evaled;
	T_ARGS(bool in):inUse(in),evaled(false){}
	void add(Statement* a){
		evals.push_back((const AbstractClass*)a);
	}
	std::vector<const AbstractClass*>& eval(RData& r, PositionID id) const;
};
const T_ARGS NO_TEMPLATE(false);
#define TemplateArg const AbstractClass*
#include "output.hpp"
#endif /* INCLUDES_HPP_ */
