/*
 * GeneratorClassP.hpp
 *
 *  Created on: Jun 24, 2014
 *      Author: Billy
 */

#ifndef GENERATORCLASSP_HPP_
#define GENERATORCLASSP_HPP_
#include "./GeneratorClass.hpp"
#include "../../ast/function/E_GEN.hpp"
GeneratorClass::GeneratorClass(const E_GEN* m, const String name, const AbstractClass* rT, const AbstractClass* tClass, const std::vector<std::pair<const AbstractClass*,String>>& args):
		AbstractClass(m->module.surroundingScope,str(name,rT, tClass, args),nullptr,PRIMITIVE_LAYOUT,CLASS_GEN,true,getGeneratorType(name, tClass, args)),innerTypes(args){
		myGen = m;
		thisClass = tClass;
		returnClass = rT;
	}



#endif /* GENERATORCLASSP_HPP_ */
