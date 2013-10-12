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
	void collectReturns(RData& r, std::vector<ClassProto*>& vals){
	}
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
		if(superC->hasFunction(right)) return returnType = functionClass;
		return returnType = superC->getDataClass(right,filePos);
	}

	ClassProto* getSelfClass() override final{
		error("Cannot get selfClass of lookup"); return NULL;
	}
	Constant* getConstant(RData& r) override final { return NULL; }
	DATA evaluate(RData& a) override{
		registerClasses(a);
		ClassProto* lT = left->checkTypes(a);
		if(lT->hasFunction(right)){
			return NULL;//lT->getFunction(right);
		}
		auto lloc = left->getLocation(a);
		if(lloc!=NULL){
			auto l =getInt32(lT->getDataClassIndex(right,filePos));
			std::vector<Value*> look = {getInt32(0),l};
			auto pos = a.builder.CreateGEP(lloc,look);
			return a.builder.CreateLoad(pos);
		}
		auto lVal = left->evaluate(a);
		if(lVal!=NULL){
			//TODO is this even close to right?
			if(lVal->getType()->isVectorTy()){
				return a.builder.CreateExtractElement(lVal, getInt(lT->getDataClassIndex(right,filePos)),"getV");
			} else if(lVal->getType()->isStructTy()){
				std::vector<unsigned int> b =  {lT->getDataClassIndex(right,filePos)};
				auto t= a.builder.CreateExtractValue(lVal,ArrayRef<unsigned int>(b),"getV");
				return t;
			} else {
				std::vector<Value*> look = {};
				error("can't fast-lookup non-vector");
				return NULL;
			//	return a.builder.CreateExtractValue(lVal, Arra)
			}
		//	return a.builder.CreateGEP(lM->llvmLocation, getInt(INTTYPE,lT->getDataClassIndex(right,filePos)));
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
		if(lT->hasFunction(right)) return NULL;
		auto lloc = left->getLocation(a);
		//TODO add additional 0 if global or pointer
		if(lloc!=NULL){
			std::vector<Value*> look = {getInt32(0),getInt32(lT->getDataClassIndex(right,filePos))};
			return a.builder.CreateGEP(lloc,look);
			//return GetElementPtrInst::Create(lM->llvmLocation, ArrayRef<Value*>(look), "lookup",a.builder.GetInsertBlock());
		} else {
			error("Could not find Value to get");
			return NULL;
		}
	};
	ReferenceElement* getMetadata(RData& r){
		auto lT = left->checkTypes(r);

		if(lT->hasFunction(right)){
			return lT->getFunction(right);
		}
		//TODO support getting class functions by moving funcMap
		return new ReferenceElement("", NULL,lT->name+operation+right, NULL, lT->getDataClass(right,filePos), funcMap(), NULL, NULL);
	}
};



#endif /* E_LOOKUP_HPP_ */
