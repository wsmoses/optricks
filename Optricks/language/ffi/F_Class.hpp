/*
 * F_Class.hpp
 *
 *  Created on: May 1, 2014
 *      Author: Billy
 */

#ifndef F_CLASS_HPP_
#define F_CLASS_HPP_
#include "../class/AbstractClass.hpp"


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

template<typename C> inline const AbstractClass* convertClassType(Scopable* s){
	cerr << "Cannot convert class (not implemented) "<< boost::units::detail::demangle(typeid(C).name()) << " to optricks" << endl << flush;
	exit(1);
}

template<typename C> const AbstractClass* const convertClass<C>::convert(Scopable* s){
	if(boost::is_union<C>::value){
		cerr << "Cannot convert union "<< boost::units::detail::demangle(typeid(C).name()) << " to optricks" << endl << flush;
		cerr << "  There are no unions on optricks (since they are inherently unsafe). Use polymorphism instead" << endl << flush;
		exit(1);
	}
	else if(boost::is_class<C>::value){
		return convertClassType<C>(s);
	}
	cerr << "Cannot convert class " << boost::units::detail::demangle(typeid(C).name()) << " to optricks" << endl << flush;
	exit(1);
}

template<> struct convertClass<void>{
	static const AbstractClass* const convert(Scopable* s){
		return &voidClass;
	}
};
template<> struct convertClass<bool>{
	static const AbstractClass* const convert(Scopable* s){
		return &boolClass;
	}
};
template<> struct convertClass<char>{
	static const AbstractClass* const convert(Scopable* s){
		return &charClass;
	}
};
template<> struct convertClass<void*>{
	static const AbstractClass* const convert(Scopable* s){
		return &c_pointerClass;
	}
};
template<typename A, size_t B> struct convertClass<A[B]>{
	static const AbstractClass* const convert(Scopable* s){
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
	static const AbstractClass* const convert(Scopable* s){
		assert(sizeof(A)+sizeof(B)==sizeof(std::pair<A,B>));
		return TupleClass::get({convertClass<A>::convert(s), convertClass<B>::convert(s)});
	}
};

template<typename... A> struct convertClass<std::tuple<A...>>{
	static const AbstractClass* const convert(Scopable* s){
		assert(totalSize<A...>::get()==sizeof(std::tuple<A...>));
		std::vector<const AbstractClass*> in(sizeof...(A));
		addClassToVector<0,A...>::add(in, s);
		return TupleClass::get(in);
	}
};

template<typename A, typename... B> struct convertClass<A(&)(B...)>{
	static const AbstractClass* const convert(Scopable* s){
		std::vector<const AbstractClass*> in(sizeof...(B));
		addClassToVector<0,B...>::add(in,s);
		return FunctionClass::get(convertClass<A>::convert(s),in);
	}
};

template<typename A, typename... B> struct convertClass<A(*)(B...)>{
	static const AbstractClass* const convert(Scopable* s){
		std::vector<const AbstractClass*> in(sizeof...(B));
		addClassToVector<0,B...>::add(in, s);
		return FunctionClass::get(convertClass<A>::convert(s),in);
	}
};

#endif /* F_CLASS_HPP_ */
