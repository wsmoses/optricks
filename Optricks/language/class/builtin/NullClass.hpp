/*
 * NullClass.hpp
 *
 *  Created on: May 12, 2014
 *      Author: Billy
 */

#ifndef NULLCLASS_HPP_
#define NULLCLASS_HPP_
#include "../AbstractClass.hpp"

class NullClass: public AbstractClass{
public:
	NullClass(bool b): AbstractClass(nullptr,"null_t",nullptr,POINTER_LAYOUT,CLASS_NULL,true,C_POINTERTYPE){
		LANG_M.addClass(PositionID(0,0,"#null"),this);
		LANG_M.addVariable(PositionID(0,0,"#null"),"null", new ConstantData(llvm::Constant::getNullValue(type), this));
	};
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
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCast(a));
		assert(hasCast(a));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		if(a->classType==CLASS_NULL && b->classType==CLASS_NULL) return 0;
		else if(a->classType==CLASS_NULL) return -1;
		else if(b->classType==CLASS_NULL) return 1;
		return 0;
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		//TODO FOR REAL
		return hasCast(toCast);
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		//TODO FOR REAL
		return toCast->classType==CLASS_NULL || toCast->classType==CLASS_VOID
				|| toCast->classType==CLASS_CPOINTER
				|| toCast->classType==CLASS_CSTRING
				|| toCast->classType==CLASS_ARRAY
				|| toCast->classType==CLASS_HASHMAP
				|| toCast->classType==CLASS_PRIORITYQUEUE
				|| toCast->classType==CLASS_FUNC


				|| toCast->layout==POINTER_LAYOUT
				|| toCast->layout==PRIMITIVEPOINTER_LAYOUT;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		//TODO FOR REAL
		if(toCast->classType==CLASS_NULL) return valueToCast;
		if(!hasCast(toCast)){
			illegalCast(id,toCast);
		}
		return llvm::Constant::getNullValue(toCast->type);
	}
};

const NullClass nullClass(true);

#endif /* NULLCLASS_HPP_ */
