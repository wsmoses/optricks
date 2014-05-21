/*
 * F_Function.hpp
 *
 *  Created on: May 11, 2014
 *      Author: Billy
 */

#ifndef F_FUNCTION_HPP_
#define F_FUNCTION_HPP_
#include "./F_Class.hpp"


template<typename R, typename... B> SingleFunction* import_c_function_h(R (*func)(B...), String name, String lib=""){
	const FunctionClass* C = convertClass<R(*)(B...)>::convert(&NS_LANG_C.staticVariables);
	std::vector<AbstractDeclaration> ad;
	for(const auto& a: C->argumentTypes)
		ad.push_back(AbstractDeclaration(a));
	return new CompiledFunction(
			new FunctionProto(name, ad, C->returnType, false),
			getRData().getExtern(name, C->returnType, C->argumentTypes, false, lib));
};

template<typename...B> struct addMangledToStream{
	void add(ostream& s);
};

template<typename A, typename...B> struct addMangledToStream<A,B...>{
	void add(ostream& s){
		s << typeid(A).name();
	}
};

template<> struct addMangledToStream<>{
	void add(ostream& s){
	}
};

template<typename R, typename... B> SingleFunction* import_cpp_function_h(R (*func)(B...), String name, String lib=""){
	const FunctionClass* C = convertClass<R(*)(B...)>::convert(&NS_LANG_CPP.staticVariables);
	std::vector<AbstractDeclaration> ad;
	for(const auto& a: C->argumentTypes)
		ad.push_back(AbstractDeclaration(a));
	std::stringstream s;
	s << "_Z";
	s << name.length();
	s << name;
	addMangledToStream<B...>::add(s);
	return new CompiledFunction(
			new FunctionProto(name, ad, C->returnType, false),
			getRData().getExtern(s.str(), C->returnType, C->argumentTypes, false, lib));
};

/*template<typename R, typename... B> SingleFunction* import_c_function_h(R (*func)(B..., ...), String name, String lib=""){
	const FunctionClass* C = convertClass<R(*)(B...)>::convert(&NS_LANG_C.staticVariables);
	std::vector<AbstractDeclaration> ad;
	for(const auto& a: C->argumentTypes)
		ad.push_back(AbstractDeclaration(a));
	return new CompiledFunction(
			new FunctionProto(name, ad, C->returnType, true),
			getRData().getExtern(name, C->returnType, C->argumentTypes, false, lib));
};*/

#define import_c_function(a) import_c_function_h(a, #a)
#define import_c_function_from(a, lib) import_c_function(a, #a, lib)
#define add_import_c_function(m, a) (m)->addFunction(PositionID("#internal",0,0), #a)->add(import_c_function(a), PositionID("#internal",0,0))
#endif /* F_FUNCTION_HPP_ */
