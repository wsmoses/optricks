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
	: AbstractClass(sc,nam,(!isObject && t==POINTER_LAYOUT && supa==nullptr)?(&objectClass):(supa),
			t,CLASS_USER, fina,
			(t==POINTER_LAYOUT)?(
					(llvm::Type*) PointerType::getUnqual(StructType::create(getGlobalContext(), StringRef(nam)))
	):(
					(llvm::Type*)StructType::create(getGlobalContext(), StringRef(nam))
			)
			),
					constructors(nam, nullptr),start(0),final(false)
	{
		if(t==PRIMITIVEPOINTER_LAYOUT) PositionID(0,0,"#class").warning("Garbage collection for primitivepointers is not implemented");
		if(superClass) assert(dynamic_cast<const UserClass*>(superClass));
		if(isObject){
			localVars.push_back(&intClass);
			final = true;
		}
	};

Value* UserClass::generateData(RData& r, PositionID id) const{
	if(!final) id.compilerError("Cannot generateData of non-finalized type");
	if(layout==PRIMITIVEPOINTER_LAYOUT || layout==PRIMITIVE_LAYOUT) return UndefValue::get(type);
	else {
		assert(type);
		assert(dyn_cast<PointerType>(type));
		auto tmp = ((PointerType*)type)->getArrayElementType();
		assert(tmp);
		assert(r.lmod);
		uint64_t s = DataLayout(r.lmod).getTypeAllocSize(tmp);
		IntegerType* ic = llvm::IntegerType::get(getGlobalContext(), 8*sizeof(size_t));
		auto v = CallInst::CreateMalloc(r.builder.GetInsertBlock(), ic,
				tmp, ConstantInt::get(ic, s));
		r.builder.Insert(v);
		return v;
	}
}


#endif /* USERCLASSP_HPP_ */
