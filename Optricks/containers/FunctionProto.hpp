/*
 * FunctionProto.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef FUNCTIONPROTO_HPP_
#define FUNCTIONPROTO_HPP_

#include "settings.hpp"
#define FUNCTIONPROTO_C_
class FunctionProto{
	public:
		String name;
		std::vector<Declaration*> declarations;
		ClassProto* returnType;
		FunctionProto(String n, std::vector<Declaration*>& a, ClassProto* r):name(n),declarations(a), returnType(r){}
		FunctionProto(String n, ClassProto* r=NULL):name(n),declarations(), returnType(r){}
		/*
		 * Checks if this can be casted to F, and how well
		 * In Declaration.hpp
		 */
		std::pair<bool,std::pair<unsigned int, unsigned int>> match(FunctionProto* func,RData& r) const;
		bool equals(const FunctionProto* f,RData& r) const;
		String toString() const;
};

#endif /* FUNCTIONPROTO_HPP_ */
