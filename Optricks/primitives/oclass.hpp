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
		bool primitive;
		oclass* outerClass;
		std::vector<Statement*> under;
		std::vector<Declaration*> data;
		ClassProto* proto;
		oclass(PositionID id, String nam, Statement* sC, Statement* loc, bool prim, oclass* outer):Statement(id, classClass),
				name(nam),superClass(sC),self(loc),primitive(prim), outerClass(outer), under(), data(), proto(NULL){
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
			for(Statement*& a: under) a->checkTypes(r);
			return returnType;
		}
		DATA evaluate(RData& r){
			registerClasses(r);
			for(Statement*& a: under) a->checkTypes(r);
			return DATA::getClass(proto);
		}
		void registerClasses(RData& r) override final{
			for(Statement*& a:under) a->registerClasses(r);
			error("Registration of classes has yet to be implemented");
			//TODO
		}
		void registerFunctionPrototype(RData& r) override final{
			for(Statement*& a:under) a->registerFunctionPrototype(r);
		}
		void buildFunction(RData& r) override final{
			for(Statement*& a:under) a->buildFunction(r);
		}
		void resolvePointers() override final{
			for(Statement*& a:under) a->resolvePointers();
		}
};

void initClasses(){
	initClassesMeta();
	ClassProto* cl[] = {classClass, objectClass, autoClass, boolClass,
			functionClass, doubleClass, complexClass, intClass, stringClass, charClass,
			sliceClass, voidClass,
			c_stringClass,c_intClass, c_longClass, c_long_longClass, c_pointerClass};
	for(ClassProto*& p:cl){
		LANG_M->addPointer(PositionID(0,0,"oclass#init"), p->name, DATA::getClass(p),classClass);
	}
}

#endif /* OCLASS_HPP_ */
