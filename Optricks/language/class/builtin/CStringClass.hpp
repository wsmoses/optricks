/*
 * CStringClass.hpp
 *
 *  Created on: May 25, 2014
 *      Author: Billy
 */

#ifndef CSTRINGCLASS_HPP_
#define CSTRINGCLASS_HPP_


#include "../AbstractClass.hpp"
#include "../ScopeClass.hpp"
class CStringClass: public AbstractClass{
public:
	inline CStringClass(bool b):
		AbstractClass(& NS_LANG_C.staticVariables,"string", NULL,PRIMITIVE_LAYOUT,CLASS_CSTRING,true,C_STRINGTYPE){
		NS_LANG_C.staticVariables.addClass(PositionID(0,0,"#cstring"),this);
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CSTRING || toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CSTRING || toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
			assert(hasCast(a));
			assert(hasCast(b));
			if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
			else if(a->classType==CLASS_VOID) return 1;
			else if(b->classType==CLASS_VOID) return -1;
			if(a->classType==CLASS_CSTRING && b->classType==CLASS_CSTRING) return 0;
			else if(a->classType==CLASS_CSTRING) return 1;
			else if(b->classType==CLASS_CSTRING) return -1;
			return 0;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType==CLASS_CPOINTER) return r.builder.CreatePointerCast(valueToCast, C_POINTERTYPE);
		if(toCast->classType!=CLASS_CSTRING) id.error("Cannot cast "+getName()+" type to "+toCast->getName());
		return valueToCast;
	}
};

const CStringClass c_stringClass(true);



#endif /* CSTRINGCLASS_HPP_ */
