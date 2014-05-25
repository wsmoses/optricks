/*
 * F_Var.hpp
 *
 *  Created on: May 11, 2014
 *      Author: Billy
 */

#ifndef F_VAR_HPP_
#define F_VAR_HPP_
#include "./F_Class.hpp"

template<typename C> struct import_c_const_h{
	static Data* import(C val);
};
template<> struct import_c_const_h<char>{
	static Data* import(char val){
		return new ConstantData(CharClass::getValue(val), &charClass);
	}
};
template<> struct import_c_const_h<bool>{
	static Data* import(bool val){
		return new ConstantData(BoolClass::getValue(val), &boolClass);
	}
};
template<typename C> Data* import_c_const_h<C>::import(C val){
	if(std::is_integral<C>::value){
		if(std::is_signed<C>::value){
			return new ConstantData(llvm::ConstantInt::getSigned(llvm::IntegerType::get(llvm::getGlobalContext(),32),(int64_t)val), convertClass<C>::convert(&NS_LANG_C.staticVariables));
		} else{
			return new ConstantData(llvm::ConstantInt::get(llvm::IntegerType::get(llvm::getGlobalContext(),32),(uint64_t)val, false), convertClass<C>::convert(&NS_LANG_C.staticVariables));
		}
	}
	cerr << "Cannot convert value " << demangle(typeid(C).name()) << " to optricks" << endl << flush;
	exit(1);
}

#define import_c_const(a) import_c_const_h<typeof(a)>::import(a)
#define add_import_c_const(m, a) (m)->addVariable(PositionID("#internal",0,0), #a, import_c_const_h<decltype(a)>::import(a))

/*
LANG_M.addVariable(PositionID(0,0,"#main"), "stdout", new ConstantData(
			getRData().builder.CreatePointerCast(new GlobalVariable(C_POINTERTYPE, false, GlobalValue::LinkageTypes::ExternalLinkage,
			nullptr,"stdout",GlobalVariable::ThreadLocalMode::NotThreadLocal,0,true
			),C_POINTERTYPE)
			, &c_pointerClass));*/
#define import_c_var(a, b) new ConstantData(\
		getRData().builder.CreatePointerCast(\
		new llvm::GlobalVariable(convertLLVM<decltype(a)>::convert(), false, llvm::GlobalValue::LinkageTypes::ExternalLinkage,\
				nullptr,#a,llvm::GlobalVariable::ThreadLocalMode::NotThreadLocal,0,true),\
	 convertClass<decltype(a)>::convert(b)->type), convertClass<decltype(a)>::convert(b))

//import_c_const_h<typeof(a)>::import(a)
#define add_import_c_var(m, a, b) (m)->addVariable(PositionID("#internal",0,0), #a, import_c_var(a, b))
#endif /* F_VAR_HPP_ */
