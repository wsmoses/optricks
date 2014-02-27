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
//#include "RData.hpp"

#define FUNCTIONPROTO_C_
class FunctionProto{
	public:
		const String name;
		const std::vector<AbstractDeclaration> declarations;
		const AbstractClass* const returnType;
		FunctionProto(String n, const std::vector<AbstractDeclaration>& a, const AbstractClass* r):name(n),declarations(a), returnType(r){}
		FunctionProto(String n, const AbstractClass* r=NULL):name(n),declarations(), returnType(r){}
		const AbstractClass* getGeneratorType();
		FunctionClass* getFunctionClass() const;
		/*
		 * Checks if this can be casted to F, and how well
		 * In Declaration.hpp
		 */
		//std::pair<bool,std::pair<unsigned int, unsigned int>> match(FunctionProto* func) const;
		bool equals(const FunctionProto* f, PositionID id) const;
		String toString() const;
		inline std::pair<bool,std::pair<unsigned int, unsigned int>> validArgumentsAre(const std::vector<Data*>& args) const;
};

#endif /* FUNCTIONPROTO_HPP_ */
