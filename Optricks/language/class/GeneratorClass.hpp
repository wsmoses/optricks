/*
 * GeneratorClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef GENERATORCLASS_HPP_
#define GENERATORCLASS_HPP_

#include "../AbstractClass.hpp"

class GeneratorClass: public AbstractClass{
public:
	static inline String str(const String nam, const AbstractClass* rT, const std::vector<std::pair<const AbstractClass*,String>>& a){
		String s= "generator{'"+nam+"',"+rT->getName();
		for(const auto& b: a){
			s+=","+b.second+":"+b.first->getName();
		}
		return s+"}";
	}
	static inline Type* getGeneratorType(const String nam, const std::vector<std::pair<const AbstractClass*,String>>& args){
		const auto len = args.size();
		if(len==0) return VOIDTYPE;
		if(len==1) return args[0].first->type;
		Type* ar[len];
		for(unsigned int i=0; i<len; i++){
			assert(args[i]->classType!=CLASS_LAZY);
			assert(args[i]->classType!=CLASS_REF);
			ar[i]=args[i].first->type;
		}
		return StructType::create(ArrayRef<Type*>(ar, len),StringRef(nam),false);
	}
	const std::vector<std::pair<const AbstractClass*,String>> innerTypes;
protected:
	GeneratorClass(const String name, const AbstractClass* rT, const std::vector<std::pair<const AbstractClass*,String>>& args):
		AbstractClass(nullptr,str(name, rT, args),nullptr,PRIMITIVE_LAYOUT,CLASS_GEN,true,getGeneratorType(name, args)),innerTypes(args){
	}
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		return toCast == this;
	}

	bool hasLocalData(String s) const override {
		for(const auto& a: innerTypes) if(a.second==s) return true;
		return false;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{

		for(const auto& a: innerTypes)
			if(a.second==s) return a.first;

			illegalLocal(id,s);
			exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		assert(instance->type==R_LOC || instance->type==R_CONST);
		assert(instance->getReturnType()==this);
		if(instance->type==R_CONST){
			Value* v = ((ConstantData*)instance)->value;

			for(unsigned i = 0; i<innerTypes.size(); i++)
				if(innerTypes[i].second==s)
					return new ConstantData(r.builder.CreateExtractElement(v,getInt32(i)),this);
			illegalLocal(id,s);
			exit(1);
		} else {
			assert(instance->type==R_LOC);
			id.compilerError("Location framework not complete -- generator");
			exit(1);
		}
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		return toCast == this;
	}
	inline Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const{
		if(toCast!=this) id.error("Cannot cast between generator objects");
		return valueToCast;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(a==this);
		assert(b==this);
		return 0;
	}
};



#endif /* GENERATORCLASS_HPP_ */
