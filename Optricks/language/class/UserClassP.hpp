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
#include "../data/ReferenceData.hpp"
#include "../data/DeclarationData.hpp"
UserClass::UserClass(const Scopable* sc, String nam, const AbstractClass* const supa, LayoutType t, bool fina,bool isObject,llvm::Type* T)
	: AbstractClass(sc,nam,(!isObject && t==POINTER_LAYOUT && supa==nullptr)?(&objectClass):(supa),
				t,CLASS_USER, fina,
				(t==PRIMITIVEPOINTER_LAYOUT)?C_POINTERTYPE:(
				(t==POINTER_LAYOUT)?(
						(llvm::Type*) llvm::PointerType::getUnqual(llvm::StructType::create(llvm::getGlobalContext(), llvm::StringRef(nam)))
		):(
				T?T:(
						(llvm::Type*)llvm::StructType::create(llvm::getGlobalContext(), llvm::StringRef(nam)))
				)
				)),
						constructors(nam, nullptr),start(0),final(false)
		{
			if(t==PRIMITIVEPOINTER_LAYOUT){
				PositionID(0,0,"#class").warning("Garbage collection for primitivepointers is not implemented");
				assert(supa==nullptr);
				localMap["#data"]=localVars.size();
				localVars.push_back(&c_pointerClass);
				final = true;
			}
			if(t!=PRIMITIVE_LAYOUT) assert(T==nullptr);
			if(T) final=true;
			if(superClass) assert(dynamic_cast<const UserClass*>(superClass));
			if(isObject){
				localVars.push_back(&intClass);
				final = true;
				assert(superClass==nullptr);
			}
		};

llvm::Value* UserClass::generateData(RData& r, PositionID id) const{
	if(!final) id.compilerError("Cannot generateData of non-finalized type");
	if(layout==PRIMITIVEPOINTER_LAYOUT || layout==PRIMITIVE_LAYOUT) return llvm::UndefValue::get(type);
	else {
		assert(type);
		assert(llvm::dyn_cast<llvm::PointerType>(type));
		auto tmp = ((llvm::PointerType*)type)->getArrayElementType();
		assert(tmp);
		return r.allocate(tmp);
	}
}
const Data* UserClass::getLocalData(RData& r, PositionID id, String s, const Data* instance) const {
	if(!final) id.compilerError("Cannot getLocalData() on unfinalized type");
	if(layout==PRIMITIVEPOINTER_LAYOUT){
		if(s!="#data") illegalLocal(id,s);
		assert(instance->type==R_DEC || instance->type==R_LOC || instance->type==R_CONST);
		assert(instance->getReturnType()==this);
		if(instance->type==R_LOC){
			return new LocationData(((LocationData*)instance)->value, &c_pointerClass);
		} else if(instance->type==R_DEC){
			return new LocationData(((DeclarationData*)instance)->value->fastEvaluate(r)->value, &c_pointerClass);
		} else {
			assert(instance->type==R_CONST);
			return new ConstantData(((ConstantData*)instance)->value, &c_pointerClass);
		}
	}
	assert(instance);
	auto tmp=this;
			do{
				auto fd = tmp->localMap.find(s);
				if(fd!=tmp->localMap.end()){
					unsigned start = tmp->start+fd->second;
					if(instance->type==R_DEC)
						instance = ((const DeclarationData*)instance)->value->fastEvaluate(r);
					assert(instance->type==R_DEC || instance->type==R_LOC || instance->type==R_CONST
							/*|| instance->type==R_REF*/);
					assert(instance->getReturnType()==this);
					if(instance->type==R_LOC){
						Location* ld;
						if(layout==PRIMITIVE_LAYOUT){
							ld = ((const LocationData*)instance)->value->getInner(r, id, 0, start);
							assert(ld);
						}
						else{
							ld = new StandardLocation(r.builder.CreateConstGEP2_32(
									((const LocationData*)instance)->value->getValue(r,id),0,start));
							assert(ld);
						}
						return new LocationData(ld, tmp->localVars[fd->second]);
					} else if(instance->type==R_DEC){
						Location* ld;
						if(layout==PRIMITIVE_LAYOUT){
							ld = ((const DeclarationData*)instance)->value->fastEvaluate(r)->value->getInner(r, id, 0, start);
							assert(ld);
						}
						else{
							ld = new StandardLocation(r.builder.CreateConstGEP2_32(
									((const DeclarationData*)instance)->value->fastEvaluate(r)->value->getValue(r,id),0,start));
							assert(ld);
						}
						return new LocationData(ld, tmp->localVars[fd->second]);
					} /*else if(instance->type==R_REF){
						Location* ld;
						const LocationData* D  = ((const ReferenceData*)instance)->value;
						cerr << demangle(typeid(D).name()) << endl << flush;
						cerr << D->value->getName() << endl << flush;
						assert(D);
						if(layout==PRIMITIVE_LAYOUT){
							ld = D->value->getInner(r, id, 0, start);
							cerr << demangle(typeid(D->value).name()) << endl << flush;
							cerr << "LAZY: " << dynamic_cast<LazyLocation*>(D->value) << " STD: " << dynamic_cast<StandardLocation*>(D->value)<< endl << flush;
							assert(ld);
						}
						else{
							ld = new StandardLocation(r.builder.CreateConstGEP2_32(
									D->value->getValue(r,id),0,start));
							assert(ld);
						}
						return new LocationData(ld, tmp->localVars[fd->second]);
					} */else{
						assert(instance->type==R_CONST);
						llvm::Value* v = ((ConstantData*)instance)->value;
						if(layout==PRIMITIVE_LAYOUT)
							return new ConstantData(r.builder.CreateExtractValue(v,start),tmp->localVars[fd->second]);
						else{
							return new LocationData(new StandardLocation(r.builder.CreateConstGEP2_32(v, 0, start)), tmp->localVars[fd->second]);
						}
					}
				}
				tmp = (UserClass*)(tmp->superClass);
			}while(tmp);
		illegalLocal(id,s);
		exit(1);
}

#endif /* USERCLASSP_HPP_ */
