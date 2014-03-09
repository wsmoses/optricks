/*
 * FunctionProtoP.hpp
 *
 *  Created on: Jan 10, 2014
 *      Author: Billy
 */

#ifndef FUNCTIONPROTOP_HPP_
#define FUNCTIONPROTOP_HPP_
#include "FunctionProto.hpp"
#include "./class/builtin/FunctionClass.hpp"
#include "./class/UserClass.hpp"

FunctionClass* FunctionProto::getFunctionClass() const{
	static FunctionClass* fc=nullptr;
	if(fc==nullptr){
		std::vector<const AbstractClass*> ac;
		for(const auto& a: declarations) ac.push_back(a.declarationType);
		fc=FunctionClass::get(returnType, ac);
	}
	return fc;
}
/*
std::pair<bool,std::pair<unsigned int, unsigned int>> FunctionProto::match(FunctionProto* func) const{
	unsigned int optional;
	if(func->declarations.size()!=declarations.size()){
		if(declarations.size()>func->declarations.size()) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		for(unsigned int a=declarations.size(); a<func->declarations.size(); ++a){
			if(func->declarations[a].defaultValue==nullptr) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		}
		optional = func->declarations.size()-declarations.size();
	} else optional = 0;
	unsigned int count=0;
	for(unsigned int a=0; a<declarations.size(); ++a){
		AbstractClass* const class1 = declarations[a].declarationType;
		AbstractClass* const class2 = func->declarations[a].declarationType;
		if(class1->classType==CLASS_VOID){
			if(func->declarations[a].defaultValue==nullptr)
				return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
		}
		else{
			auto t = class1->compatable(class2);
			if(!t.first)  return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
			else{
				if(t.second>0) count++;
			}
		}
	}
	return std::pair<bool,std::pair<unsigned int, unsigned int> >(true,std::pair<unsigned int, unsigned int>(optional,count));
}*/

bool FunctionProto::equals(const FunctionProto* f, PositionID id) const{
	if(declarations.size()!=f->declarations.size()) return false;
	for(unsigned int i = 0; i<declarations.size(); ++i){
		const AbstractClass* class1 = declarations[i].declarationType;
		const AbstractClass* class2 = f->declarations[i].declarationType;
		if(class1!=class2)
			return false;
	}
	return true;
}

String FunctionProto::toString() const{
	String t = name+"(";
	bool first = true;
	for(const auto& a: declarations){
		if(first){
			first = false;
		} else t+=",";
		t+=a.declarationType->getName();
	}
	return t+")";
}

const AbstractClass* FunctionProto::getGeneratorType(){
	static UserClass* generatorType=nullptr;
	if(generatorType==nullptr){
		generatorType = new UserClass(NULL,name,NULL,PRIMITIVE_LAYOUT,true);
		for(const auto& a: declarations){
			generatorType->addLocalVariable(PositionID(0,0,"<start.getTypedef>"), a.declarationVariable, a.declarationType);
		}
	}
	return generatorType;
}


#endif /* FUNCTIONPROTOP_HPP_ */