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
			left(a), right(b), operation(o){};

	void write(ostream& f,String a="") const override{
		f << left;
		f << operation;
		f << right;
	}

	String getFullName() override final{
		return left->getFullName()+operation+right;
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
	ClassProto* checkTypes(RData& r){
		ClassProto* superC = left->checkTypes(r);
		return returnType = superC->getDataClass(right,filePos);
	}
	DATA evaluate(RData& a) override{
		auto lT = left->checkTypes(a);
		auto lloc = left->getLocation(a);
		if(lloc!=NULL){
			std::vector<Value*> look = {ConstantInt::get(INTTYPE,0),ConstantInt::get(INTTYPE,lT->getDataClassIndex(right,filePos))};
			auto pos = a.builder.CreateGEP(lloc,look);
			return a.builder.CreateLoad(pos);
		}
		auto lVal = left->evaluate(a);
		if(lVal!=NULL){
			//TODO is this even close to right?
			if(lVal->getType()->isVectorTy()){
				return a.builder.CreateExtractElement(lVal, ConstantInt::get(INTTYPE,lT->getDataClassIndex(right,filePos)),"getV");
			} else {
				std::vector<Value*> look = {};
				error("can't fast-lookup non-vector");
				return NULL;
			//	return a.builder.CreateExtractValue(lVal, Arra)
			}
		//	return a.builder.CreateGEP(lM->llvmLocation, ConstantInt::get(INTTYPE,lT->getDataClassIndex(right,filePos)));
			//TODO (check if vector or struct) return a.builder.CreateExtractValue(lM->llvmObject, ArrayRef<Value*>(look), "lookup2");
	//		return GetElementPtrInst::Create(lM->llvmObject,ArrayRef<Value*>(look), "lookup2",a.builder.GetInsertBlock());
		} else {
			error("Could not find Value to get");
			return NULL;
		}
	}
	Statement* simplify() override{
		return this;
	}
	Value* getLocation(RData& a) override final {
		auto lT = left->checkTypes(a);
		auto lloc = left->getLocation(a);
		//TODO add additional 0 if global or pointer
		if(lloc!=NULL){
			std::vector<Value*> look = {ConstantInt::get(INTTYPE,0),ConstantInt::get(INTTYPE,lT->getDataClassIndex(right,filePos))};
			return a.builder.CreateGEP(lloc,look);
			//return GetElementPtrInst::Create(lM->llvmLocation, ArrayRef<Value*>(look), "lookup",a.builder.GetInsertBlock());
		} else {
			error("Could not find Value to get");
			return NULL;
		}
	};
	ReferenceElement* getMetadata(RData& r){
		auto lT = left->checkTypes(r);
		//TODO support getting class functions by moving funcMap
		return new ReferenceElement("", NULL,lT->name+operation+right, NULL, lT->getDataClass(right,filePos), funcMap(), NULL, NULL);
	}
};



#endif /* E_LOOKUP_HPP_ */
