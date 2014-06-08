/*
 * F_Class.hpp
 *
 *  Created on: May 1, 2014
 *      Author: Billy
 */

#ifndef F_CLASS_HPP_
#define F_CLASS_HPP_
#include "../class/AbstractClass.hpp"
#include "../class/ScopeClass.hpp"

//#define NAME(C) demangle(typeid(C).name())
template<typename C> struct getName{
	static const String get();
};
#define DEF(A)\
template<> struct getName<A>{\
	static const String get(){\
		return #A;\
	}\
};
DEF(unsigned int)
DEF(int)
DEF(long)
DEF(long long)
DEF(size_t)
DEF(short)
DEF(float)
DEF(double)
DEF(void*)
#undef DEF
template<typename A> struct getName<const A>{
	static const String get(){
		return getName<A>::get();
	}
};
/*
template<typename A> struct getName<signed A>{
	static const String get(){
		return getName<A>::get();
	}
};
template<typename A> struct getName<unsigned A>{
	static const String get(){
		return getName<A>::get();
	}
};*/
#define NAME(C) getName<C>::get()

template<typename F, size_t s> struct getFunctionArg;

template<size_t s, typename R, typename B1, typename... B> struct getFunctionArg<R(B1, B...),s> {
	typedef typename getFunctionArg<R(B...),s-1>::value value;
};

template<typename R, typename B1, typename... B> struct getFunctionArg<R(B1, B...),0> {
	typedef B1 value;
};

template<typename C> struct convertClass{
	static const AbstractClass* const convert(Scopable* s);
};

template<> struct convertClass<void>{
	static const VoidClass* const convert(Scopable* s){
		return &voidClass;
	}
};
template<> struct convertClass<bool>{
	static const BoolClass* const convert(Scopable* s){
		return &boolClass;
	}
};
template<> struct convertClass<decltype(errno)>{
	static const IntClass* const convert(Scopable* s){
		if(sizeof(decltype(errno))==sizeof(int))
			return &c_intClass;
		if(sizeof(decltype(errno))==sizeof(long))
			return &c_longClass;
		if(sizeof(decltype(errno))==sizeof(long long))
			return &c_longlongClass;
		else{
			static IntClass ic(&(NS_LANG_C.staticVariables), "errno_t", 8*sizeof(decltype(errno)));
			return &ic;
		}
	}
};
template<> struct convertClass<int>{
	static const IntClass* const convert(Scopable* s){
		return &c_intClass;
	}
};
template<> struct convertClass<long>{
	static const IntClass* const convert(Scopable* s){
		return &c_longClass;
	}
};
template<> struct convertClass<long long>{
	static const IntClass* const convert(Scopable* s){
		return &c_longlongClass;
	}
};
template<> struct convertClass<float>{
	static const FloatClass* const convert(Scopable* s){
		return &c_floatClass;
	}
};
template<> struct convertClass<double>{
	static const FloatClass* const convert(Scopable* s){
		return &c_doubleClass;
	}
};
template<> struct convertClass<char>{
	static const CharClass* const convert(Scopable* s){
		return &charClass;
	}
};
template<> struct convertClass<void const*>{
	static const CPointerClass* const convert(Scopable* s){
		return &c_pointerClass;
	}
};
template<> struct convertClass<void*>{
	static const CPointerClass* const convert(Scopable* s){
		return &c_pointerClass;
	}
};
template<typename A> struct convertClass<const A>{
	static const ArrayClass* const convert(Scopable* s){
		return convertClass<A>::convert(s);
	}
};
template<typename A, size_t B> struct convertClass<A[B]>{
	static const ArrayClass* const convert(Scopable* s){
		return ArrayClass::get(convertClass<A>::convert(s), B);
	}
};
template<typename... A> struct totalSize{
	size_t get();
};
template<typename A, typename...B> struct totalSize<A,B...>{
	size_t get(){
		return sizeof(A)+totalSize<B...>();
	}
};
template<> struct totalSize<>{
	size_t get(){
		return 0;
	}
};

template<size_t s, typename... F> struct addClassToVector{
	static void add(std::vector<const AbstractClass*>& v, Scopable* sc);
};

template<size_t s, typename A, typename... F> struct addClassToVector<s, A, F...>{
	static void add(std::vector<const AbstractClass*>& v, Scopable* sc){
		v[s] = convertClass<A>::convert(sc);
		addClassToVector<s+1, F...>::add(v, sc);
	}
};

template<size_t s> struct addClassToVector<s>{
	static void add(std::vector<const AbstractClass*> in, Scopable* sc){}
};

template<typename A, typename B> struct convertClass<std::pair<A,B>>{
	static const TupleClass* const convert(Scopable* s){
		assert(sizeof(A)+sizeof(B)==sizeof(std::pair<A,B>));
		std::vector<const AbstractClass*> in(2);
		addClassToVector<0,A,B>::add(in, s);
		return TupleClass::get(in);
	}
};

template<typename... A> struct convertClass<std::tuple<A...>>{
	static const TupleClass* const convert(Scopable* s){
		assert(totalSize<A...>::get()==sizeof(std::tuple<A...>));
		std::vector<const AbstractClass*> in(sizeof...(A));
		addClassToVector<0,A...>::add(in, s);
		return TupleClass::get(in);
	}
};

template<typename A, typename... B> struct convertClass<A(&)(B...)>{
	static const FunctionClass* const convert(Scopable* s){
		std::vector<const AbstractClass*> in(sizeof...(B));
		addClassToVector<0,B...>::add(in,s);
		return FunctionClass::get(convertClass<A>::convert(s),in);
	}
};

template<typename A, typename... B> struct convertClass<A(*)(B...)>{
	static const FunctionClass* const convert(Scopable* s){
		std::vector<const AbstractClass*> in(sizeof...(B));
		addClassToVector<0,B...>::add(in, s);
		return FunctionClass::get(convertClass<A>::convert(s),in);
	}
};

template<typename C> inline const AbstractClass* convertClassType(Scopable* s){
	cerr << "Cannot convert class (not implemented) "<< NAME(C) << " to optricks" << endl << flush;
	exit(1);
}

template<typename C> const AbstractClass* const convertClass<C>::convert(Scopable* s){
	if(std::is_union<C>::value){
		cerr << "Cannot convert union "<< NAME(C) << " to optricks" << endl << flush;
		cerr << "  There are no unions in optricks (since they are inherently unsafe). Use polymorphism instead" << endl << flush;
		exit(1);
	} else if(std::is_integral<C>::value){
		/*if(!std::is_signed<C>::value){
			cerr << "Cannot convert unsigned integer "<< st << " to optricks" << endl << flush;
			cerr << "  There are no unsigned types in optricks (since they are unsafe)." << endl << flush;
			exit(1);
		}*/
		String s = NAME(C);
		for(unsigned i=0; i<s.size(); i++)
			if(s[i]==' ') s[i]='_';
		if(NS_LANG_C.staticVariables.existsHere(s)){
			auto T = NS_LANG_C.staticVariables.getClass(PositionID(0,0,"#has"),s,NO_TEMPLATE);
			assert(T->classType==CLASS_INT);
			assert(((const IntClass*)T)->getWidth()==8*sizeof(C));
			return T;
		}
		return new IntClass(&(NS_LANG_C.staticVariables), s, 8*sizeof(C));
	} else if(std::is_floating_point<C>::value){

	} else if(std::is_member_object_pointer<C>::value){

	//} else if(std::is_function_member_pointer<C>::value){

	} else if(std::is_class<C>::value){
		return convertClassType<C>(s);
	} else if(std::is_pointer<C>::value){
		cerr << "Warning: Cannot convert class " << NAME(C) << " to optricks" << endl << flush;
		return &c_pointerClass;

	}
	cerr << "Cannot convert class " << NAME(C) << " to optricks" << endl << flush;

	//if(typeid(C)==typeid(decltype(errno)))
	//	return &c_intClass;
	exit(1);
}
//////////////////////////////////////////////////////// LLVM //////////////////////////////////////////////////////////
template<typename C> struct convertLLVM{
	static llvm::Type* const convert();
};

template<> struct convertLLVM<void>{
	static llvm::Type* const convert(){
		return llvm::Type::getVoidTy(llvm::getGlobalContext());
	}
};
//TODO should this be
template<> struct convertLLVM<bool>{
	static llvm::IntegerType* const convert(){
		return llvm::IntegerType::get(llvm::getGlobalContext(), sizeof(bool)*8);
	}
};
template<> struct convertLLVM<float>{
	static llvm::Type* const convert(){
		return llvm::Type::getFloatTy(llvm::getGlobalContext());
	}
};
template<> struct convertLLVM<double>{
	static llvm::Type* const convert(){
		return llvm::Type::getFloatTy(llvm::getGlobalContext());
	}
};
template<> struct convertLLVM<char>{
	static llvm::Type* const convert(){
		return llvm::IntegerType::get(llvm::getGlobalContext(), sizeof(char)*8);
	}
};
template<> struct convertLLVM<void const*>{
	static llvm::PointerType* const convert(){
		return C_POINTERTYPE;
	}
};
template<> struct convertLLVM<void* const>{
	static llvm::PointerType* const convert(){
		return C_POINTERTYPE;
	}
};
template<> struct convertLLVM<void*>{
	static llvm::PointerType* const convert(){
		return C_POINTERTYPE;
	}
};

template<typename A> struct convertLLVM<A*>{
	static llvm::PointerType* const convert(){
		if(std::is_enum<A>::value ||
				std::is_union<A>::value ||
				std::is_class<A>::value){
			return C_POINTERTYPE;
		}
		return llvm::PointerType::getUnqual(convertLLVM<A>::convert());
	}
};
template<typename A> struct convertLLVM<A* const>{
	static llvm::PointerType* const convert(){
		if(std::is_enum<A>::value ||
				std::is_union<A>::value ||
				std::is_class<A>::value){
			return C_POINTERTYPE;
		}
		return llvm::PointerType::getUnqual(convertLLVM<A>::convert());
	}
};

template<size_t M, size_t s, typename... F> struct addLLVMToVector{
	static void add(llvm::SmallVector<llvm::Type*,M>& v);
};

template<size_t M, size_t s, typename A, typename... F> struct addLLVMToVector<M, s, A, F...>{
	static void add(llvm::SmallVector<llvm::Type*,M>& v){
		v[s] = convertLLVM<A>::convert();
		addLLVMToVector<M, s+1, F...>::add(v);
	}
};

template<size_t M, size_t s> struct addLLVMToVector<M, s>{
	static void add(llvm::SmallVector<llvm::Type*,M> in){}
};
template<typename A, typename... B> struct convertLLVM<A(B...)>{
	static llvm::FunctionType* const convert(){
		std::vector<const AbstractClass*> in(sizeof...(B));
		llvm::SmallVector<llvm::Type*,sizeof...(B)> ar(sizeof...(B));
		addLLVMToVector<sizeof...(B), 0,B...>::add(in);
		return llvm::FunctionType::get(convertLLVM<A>::convert(),ar,false);
	}
};

template<typename A, typename... B> struct convertLLVM<A(*)(B...)>{
	static llvm::PointerType* const convert(){
		std::vector<const AbstractClass*> in(sizeof...(B));
		llvm::SmallVector<llvm::Type*,sizeof...(B)> ar(sizeof...(B));
		addLLVMToVector<sizeof...(B), 0,B...>::add(in);
		return llvm::PointerType::getUnqual(llvm::FunctionType::get(convertLLVM<A>::convert(),ar,false));
	}
};


template<> struct convertLLVM<decltype(errno)>{
	static llvm::IntegerType* const convert(){
		return llvm::IntegerType::get(llvm::getGlobalContext(), sizeof(decltype(errno)));
	}
};

template<typename A> struct convertLLVM<const A>{
	static llvm::Type* const convert(){
		return convertLLVM<A>::convert();
	}
};
template<typename C> llvm::Type* const convertLLVM<C>::convert(){
	if(std::is_union<C>::value){
		cerr << "Cannot convert union "<< NAME(C) << " to LLVM" << endl << flush;
		cerr << "  There are no unions in optricks (since they are inherently unsafe). Use polymorphism instead" << endl << flush;
		exit(1);
	} else if(std::is_integral<C>::value){
		return llvm::IntegerType::get(llvm::getGlobalContext(), sizeof(C));
	} else if(std::is_floating_point<C>::value){

	} else if(std::is_member_object_pointer<C>::value){

	//} else if(std::is_function_member_pointer<C>::value){

	} else if(std::is_class<C>::value){
	} else if(std::is_pointer<C>::value){
	}
	cerr << "Cannot convert class " << NAME(C) << " to LLVM" << endl << flush;
	exit(1);
}
#define convertClass(A,s) convertClass<A>::convert(s)


#endif /* F_CLASS_HPP_ */
