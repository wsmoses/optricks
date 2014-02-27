/*
 * UserClassP.hpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Billy
 */

#ifndef USERCLASSP_HPP_
#define USERCLASSP_HPP_
#include "./UserClass.hpp"
#include "./AbstractClass.hpp"
#include "./builtin/IntClass.hpp"
UserClass::UserClass(const Scopable* sc, String nam, const AbstractClass* const supa, LayoutType t, bool fina,bool isObject)
	: AbstractClass(sc,nam,(!isObject && t==POINTER_LAYOUT && supa==nullptr)?(objectClass):(supa),
			t,CLASS_USER, fina,
			(t==POINTER_LAYOUT)?(
					(llvm::Type*) PointerType::getUnqual(StructType::create(getGlobalContext(), nam))
	):(
					(llvm::Type*)StructType::create(getGlobalContext(), nam)
			)
			),
					start(0),final(false)
	{
		assert(t==PRIMITIVE_LAYOUT || t==POINTER_LAYOUT);
		if(superClass) assert(dynamic_cast<const UserClass*>(superClass));
		if(isObject){
			localVars.push_back(uint32Class);
			final = true;
		}
	};



#endif /* USERCLASSP_HPP_ */
