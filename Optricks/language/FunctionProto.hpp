/*
 * FunctionProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef FUNCTIONPROTO_HPP_
#define FUNCTIONPROTO_HPP_
#include "includes.hpp"
#include "AbstractDeclaration.hpp"
#include "./class/AbstractClass.hpp"
//#include "RData.hpp"

#define FUNCTIONPROTO_C_
class FunctionProto{
	public:
		const String name;
		const std::vector<AbstractDeclaration> declarations;
		const AbstractClass* const returnType;
		bool varArg;
		FunctionProto(String n, const std::vector<AbstractDeclaration>& a, const AbstractClass* r,bool va=false);
		FunctionProto(String n, const AbstractClass* r=nullptr,bool va=false);
		FunctionClass* getFunctionClass() const;
		/*
		 * Checks if this can be casted to F, and how well
		 * In Declaration.hpp
		 */
		//std::pair<bool,std::pair<unsigned int, unsigned int>> match(FunctionProto* func) const;
		bool equals(const FunctionProto* f, PositionID id) const;
		String toString() const;
};

#endif /* FUNCTIONPROTO_HPP_ */
