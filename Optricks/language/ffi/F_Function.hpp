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
	const FunctionClass* C = convertClass(R(*)(B...),&NS_LANG_C.staticVariables);
	std::vector<AbstractDeclaration> ad;
	for(const auto& a: C->argumentTypes)
		ad.push_back(AbstractDeclaration(a));
	return new CompiledFunction(
			new FunctionProto(name, ad, C->returnType, false),
			rdata.getExtern(name, C->returnType, C->argumentTypes, false, lib));
};

/*
template<typename...B> struct addMangledToStream{
	static void add(ostream& s);
};

template<typename A, typename...B> struct addMangledToStream<A,B...>{
	static void add(ostream& s){
		s << typeid(A).name();
		addMangledToStream<B...>::add(s);
	}
};

template<> struct addMangledToStream<>{
	static void add(ostream& s){
	}
};*/

/*
llvm::StringRef getMangledName(clang::GlobalDecl GD) {
   const auto *ND = llvm::cast<clang::NamedDecl>(GD.getDecl());

   llvm::StringRef &Str = MangledDeclNames[GD.getCanonicalDecl()];
   if (!Str.empty())
     return Str;

   if (!getCXXABI().getMangleContext().shouldMangleDeclName(ND)) {
     clang::IdentifierInfo *II = ND->getIdentifier();
     assert(II && "Attempt to mangle unnamed decl.");

     Str = II->getName();
     return Str;
   }

   llvm::SmallString<256> Buffer;
   llvm::raw_svector_ostream Out(Buffer);
   if (const auto *D = llvm::dyn_cast<clang::CXXConstructorDecl>(ND))
     getCXXABI().getMangleContext().mangleCXXCtor(D, GD.getCtorType(), Out);
   else if (const auto *D = llvm::dyn_cast<CXXDestructorDecl>(ND))
     getCXXABI().getMangleContext().mangleCXXDtor(D, GD.getDtorType(), Out);
   else
     getCXXABI().getMangleContext().mangleName(ND, Out);

   // Allocate space for the mangled name.
   Out.flush();
   size_t Length = Buffer.size();
   char *Name = MangledNamesAllocator.Allocate<char>(Length);
   std::copy(Buffer.begin(), Buffer.end(), Name);

   Str = llvm::StringRef(Name, Length);

	return Str;
 }*/











template<typename R, typename... B> SingleFunction* import_cpp_function_h(R (*func)(B...), String name, Scopable* scope=nullptr, String lib="",PositionID id=PositionID("#internalCPP",0,0)){
	const FunctionClass* C = convertClass(R(*)(B...),&NS_LANG_CPP.staticVariables);
	std::vector<AbstractDeclaration> ad;
	for(const auto& a: C->argumentTypes)
		ad.push_back(AbstractDeclaration(a));
	/*std::stringstream nname;
	int idx=0;
	while(idx< name.length()){
		if(name[idx]==':') idx++;
	}
	while(idx < name.length()){
		int st = idx;
		while(idx<name.length() && name[idx]!=':') idx++;
		auto tmp = name.substr(st, idx-st);
		if(idx<name.length() && name[idx]!=':'){
			idx++;
			while(idx<name.length() && name[idx]!=':') idx++;
		} else {

		}
	}*/
	std::stringstream s;
	cerr << (void*)(func) << " or " << (void*)(std::terminate) << " = " << ((void*)func==(void*)std::terminate) << endl << flush;
	if((void*)func == (void*)std::terminate){
		s << "_ZSt9terminatev";
		cerr << name << " is now terminate" << endl << flush;
		name = "terminate";
	} else{
		s << "_Z";
		s << name.length();
		s << name;
		//addMangledToStream<B...>::add(s);
		//TODO
	}
	cerr << name << " vs " << s.str() << endl << flush;
	auto CF = new CompiledFunction(
			new FunctionProto(name, ad, C->returnType, false),
			rdata.getExtern(s.str(), C->returnType, C->argumentTypes, false, lib));
	if(scope){
		scope->addFunction(id, name)->add(CF, id);
	}
	return CF;
};

/*template<typename R, typename... B> SingleFunction* import_c_function_h(R (*func)(B..., ...), String name, String lib=""){
	const FunctionClass* C = convertClass(R(*)(B...),&NS_LANG_C.staticVariables);
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

#define import_cpp_function(a) import_cpp_function_h(a, #a)
#define import_cpp_function_from(a, lib) import_cpp_function_h(a, #a, nullptr,lib)
#define add_import_cpp_function(m, a) import_cpp_function_h(a, #a, m)

#endif /* F_FUNCTION_HPP_ */
