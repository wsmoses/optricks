/*
 * E_LOOKUP.hpp
 *
 *  Created on: Apr 16, 2013
 *      Author: wmoses
 */

#ifndef E_LOOKUP_HPP_
#define E_LOOKUP_HPP_

#include "../constructs/Statement.hpp"
//TODO only applies to data, allow for functions
class E_LOOKUP : public Statement{
public:
	const Token getToken() const override{ return T_LOOKUP; }
	Statement* left;
	String right;
	String operation;
	virtual ~E_LOOKUP(){};
	E_LOOKUP(PositionID id, Statement* a,  String b, String o): Statement(id),
			left(a), right(b), operation(o){};//TODO allow more detail

	void write(ostream& f,String a="") const override{
		f << left;
		f << operation;
		f << right;
	}

	void registerClasses(RData& r) override final{
		left->registerClasses(r);
	}
	void registerFunctionArgs(RData& r) override final{
		left->registerFunctionArgs(r);
	};
	void registerFunctionDefaultArgs() override final{
		left->registerFunctionDefaultArgs();
	};
	void resolvePointers() override final{
		left->resolvePointers();
	}
	ClassProto* checkTypes(){
		ClassProto* superC = left->checkTypes();
		return returnType = superC->getDataClass(right,filePos);
	}
	DATA evaluate(RData& a) override{
		return getMetadata()->getValue(a);
	}
	Statement* simplify() override{
		return this;
	}
	Value* getLocation() override final {
		return getMetadata()->llvmLocation;
		auto lT = left->checkTypes();
		auto lM = left->getMetadata();
		//TODO add additional 0 if global or pointer
		std::vector<Value*> look = {ConstantInt::get(INTTYPE,0),ConstantInt::get(INTTYPE,lT->getDataClassIndex(right,filePos))};
		if(lM->llvmLocation!=NULL){
			//TODO is this even close to right?
			return GetElementPtrInst::Create(lM->llvmLocation, ArrayRef<Value*>(look), "lookup");
		} else if(lM->llvmObject!=NULL){
			return GetElementPtrInst::Create(lM->llvmObject, ArrayRef<Value*>(look), "lookup2");
		} else {
			error("Could not find Value to get");
			return NULL;
		}
	};
	ReferenceElement* getMetadata(){
		auto lT = left->checkTypes();
		auto lM = left->getMetadata();
		return new ReferenceElement(NULL,lT->name+operation+right, NULL, lT->getDataClass(right,filePos), NULL, NULL, NULL);
	}
};



#endif /* E_LOOKUP_HPP_ */
