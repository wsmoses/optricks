/*
 * oclass.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "oobjectproto.hpp"
#include "../constructs/Module.hpp"


#define OCLASS_P_
class oclass: public Statement
{
	public:
		String name;
		Statement* superClass;
		Statement* self;
		int storageType;
		oclass* outerClass;
		std::vector<Statement*> under;
		std::vector<Declaration*> data;
		ClassProto* proto;
		bool buildF,checkT,eval,registerF;
		oclass(PositionID id, String nam, Statement* sC, Statement* loc, int type, oclass* outer):Statement(id, classClass),
				name(nam),superClass(sC),self(loc),storageType(type), outerClass(outer), under(), data(), proto(NULL){
			buildF = checkT = eval = registerF = false;
		}
		ClassProto* getSelfClass(RData &r) override {
			if(proto!=NULL) return proto;
			else{
				registerClasses(r);
				return proto;
			}
		}
		String getFullName() override{
			if(outerClass==NULL) return name;
			else return outerClass->getFullName()+name;
		}
		ReferenceElement* getMetadata(RData& r) override{
			registerClasses(r);
			return self->getMetadata(r);
		}
		oclass* simplify() override final{
			return this;
		}
		void write(ostream& ss, String b) const override{
			ss << "class<" + name + ">";
		};
		const Token getToken() const override final {
			return T_CLASS;
		}

		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
		ClassProto* checkTypes(RData& r) override final{
			if(!checkT){
				checkT = true;
				for(Statement*& a: under) a->checkTypes(r);
			}
			return returnType;
		}
		DATA evaluate(RData& r){
			if(proto==NULL) registerClasses(r);
			if(!eval){
				eval = true;
				for(Statement*& a: under) a->evaluate(r);
			}
			return DATA::getClass(proto);
		}
		void registerClasses(RData& r) override final{
			if(proto==NULL){
				proto = new ClassProto((superClass==NULL)?NULL:(superClass->getSelfClass(r)), name, (storageType==2)?C_POINTERTYPE:NULL, storageType>0);
				if(self!=NULL) self->getMetadata(r)->llvmObject = DATA::getClass(proto);
				for(Statement*& a:under) a->registerClasses(r);
				if(storageType==2){
					if(data.size()>0) error("Cannot have data inside class with data layout of primitive_pointer");
				}
				else if(storageType==1){
					error("Registration of classes has yet to be implemented");
				} else if(storageType==0){
					for(Declaration*& d:data){
						proto->addElement(d->variable->pointer->name, d->checkTypes(r),filePos);
					}
					//TODO allow default in constructor
				}
				//TODO
			}
		}
		void registerFunctionPrototype(RData& r) override final{
			if(!registerF){
				registerF = true;
				for(Statement*& a:under) a->registerFunctionPrototype(r);
			}
		}
		void buildFunction(RData& r) override final{
			if(!buildF){
				buildF = true;
				for(Statement*& a:under) a->buildFunction(r);
			}
		}
};

void initClasses(){
	initClassesMeta();
	ClassProto* cl[] = {classClass, objectClass, autoClass, boolClass,
			functionClass, doubleClass, complexClass, intClass, stringClass, charClass,
			sliceClass, voidClass,
			c_stringClass,c_intClass, c_longClass, c_long_longClass, c_pointerClass};
	for(ClassProto*& p:cl){
		LANG_M->addPointer(PositionID(0,0,"oclass#init"), p->name, DATA::getClass(p));
	}
}

#endif /* OCLASS_HPP_ */
