/*
 * GeneratorClass.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef GENERATORCLASS_HPP_
#define GENERATORCLASS_HPP_

#include "./AbstractClass.hpp"
//TODO COMPLETE AND ADD R_DEC
class GeneratorClass: public AbstractClass{
public:
	static inline String str(const String nam, const AbstractClass* rT, const AbstractClass* tC, const std::vector<std::pair<const AbstractClass*,String>>& a){
		String s= "generator{'";
		if(tC) s+=tC->getName();
		s+= nam+"',"+rT->getName();
		for(const auto& b: a){
			s+=","+b.second+":"+b.first->getName();
		}
		return s+"}";
	}
	static inline llvm::Type* getGeneratorType(const String nam, const AbstractClass* tC, const std::vector<std::pair<const AbstractClass*,String>>& args){
		auto len = args.size();
		if(tC) len++;
		if(len==0) return VOIDTYPE;
		if(len==1){
			if(tC) return tC->type;
			else return args[0].first->type;
		}
		llvm::SmallVector<llvm::Type*,0> ar(len);
		if(tC) ar[0] = tC->type;
		for(unsigned int i=0; i<len-(tC?1:0); i++){
			assert(args[i].first->classType!=CLASS_LAZY);
			assert(args[i].first->classType!=CLASS_REF);
			ar[i+(tC?1:0)]=args[i].first->type;
		}
		return llvm::StructType::create(ar,llvm::StringRef(nam),false);
	}
	const std::vector<std::pair<const AbstractClass*,String>> innerTypes;
	const AbstractClass* thisClass;
protected:
	GeneratorClass(E_GEN* m, const String name, const AbstractClass* rT, const AbstractClass* tClass, const std::vector<std::pair<const AbstractClass*,String>>& args):
		AbstractClass(nullptr,str(name, rT, tClass, args),nullptr,PRIMITIVE_LAYOUT,CLASS_GEN,true,getGeneratorType(name, tClass, args)),innerTypes(args){
		myGen = m;
		thisClass = tClass;
	}
public:
	E_GEN* myGen;
	inline bool hasCast(const AbstractClass* const toCast) const{
		return toCast == this || toCast->classType==CLASS_VOID;
	}

	bool hasLocalData(String s) const override {
		for(const auto& a: innerTypes) if(a.second==s) return true;
		return false;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s=="this"){
			if(!thisClass){
				illegalLocal(id,s);
				return &voidClass;
			} else return thisClass;
		}
		for(const auto& a: innerTypes)
			if(a.second==s) return a.first;

		illegalLocal(id,s);
		return &voidClass;
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		assert(instance->type==R_LOC || instance->type==R_CONST);
		assert(instance->getReturnType()==this);
		if(instance->type==R_CONST){
			llvm::Value* v = ((ConstantData*)instance)->value;
			if(s=="this"){
				if(!thisClass){
					illegalLocal(id,s);
					return &VOID_DATA;
				} else return new ConstantData(r.builder.CreateExtractElement(v,getInt32(0)),this);
			}

			for(unsigned i = 0; i<innerTypes.size(); i++)
				if(innerTypes[i].second==s)
					return new ConstantData(r.builder.CreateExtractElement(v,getInt32(i+(thisClass?1:0))),this);
			illegalLocal(id,s);
			return &VOID_DATA;
		} else {
			assert(instance->type==R_LOC);
			id.compilerError("Location framework not complete -- generator");
			exit(1);
		}
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		return toCast == this || toCast->classType==CLASS_VOID;
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast!=this) id.error("Cannot cast between generator objects");
		return valueToCast;
	}

	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return 0;
	}
};



#endif /* GENERATORCLASS_HPP_ */
