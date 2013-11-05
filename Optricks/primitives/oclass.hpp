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
		ReferenceElement* self;
		LayoutType layoutType;
		oclass* outerClass;
		std::vector<Statement*> under;
		std::vector<Declaration*> data;
		ClassProto* proto;
		bool buildF,checkT,eval,registerF;
		oclass(PositionID id, String nam, Statement* sC, ReferenceElement* loc, LayoutType type, oclass* outer):Statement(id, classClass),
				name(nam),superClass(( (type==POINTER_LAYOUT) && (sC==NULL) )?(new ClassProtoWrapper(objectClass)):sC),self(loc),layoutType(type), outerClass(outer), under(), data(), proto(NULL){
			buildF = checkT = eval = registerF = false;
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata of oclass");
			registerClasses(r);
			//TODO make resolvable for class with static-functions / constructors
			return new ReferenceElement("",NULL,proto->name, DATA::getClass(proto), funcMap());
		}
		String getFullName() override{
			if(outerClass==NULL) return name;
			else return outerClass->getFullName()+name;
		}
		oclass* simplify() override final{
			return this;
		}
		void write(ostream& ss, String b) const override{
			ss << "class ";
			if(layoutType==PRIMITIVE_LAYOUT) ss << "primitive ";
			else if(layoutType==PRIMITIVEPOINTER_LAYOUT) ss << "primitive_pointer ";
			ss << name;
			if(superClass!=NULL && superClass->getToken()!=T_VOID){
				ss << " : ";
				superClass->write(ss, b);
			}
			ss << "{\n";
			String c = b;
			b+="  ";
			for(auto& a:data){
				ss << b;
				a->write(ss, b);
				ss << ";\n";
			}
			for(auto& a:under){
				ss << b;
				a->write(ss, b);
				ss << ";\n";
			}
			ss << c << "}";
		};
		const Token getToken() const override final {
			return T_CLASS;
		}

		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
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
		ClassProto* getSelfClass(RData& r) override final{
			if(proto==NULL) registerClasses(r);
			return proto;
		}
		void registerClasses(RData& r) override final{
			if(proto==NULL){
				ClassProto* super = (superClass==NULL)?NULL:(superClass->getSelfClass(r));
				proto = new ClassProto(super, name, (layoutType==PRIMITIVEPOINTER_LAYOUT)?C_POINTERTYPE:NULL, layoutType,false);
				if(self!=NULL){
					self->setObject(DATA::getClass(proto));
					if(outerClass!=NULL){
						assert(outerClass->proto);
						outerClass->proto->addClass(name,self,filePos);
					}
				}
				for(Statement*& a:under){
					a->registerClasses(r);
				}
				if(super!=NULL && super->layoutType!=layoutType) error("Cannot have a class with a superclass of a different layout type "+super->name+" "+name+" "+str<LayoutType>(layoutType));

				if(layoutType==PRIMITIVEPOINTER_LAYOUT){
					if(data.size()>0) error("Cannot have data inside class with data layout of primitive_pointer");
				}
				else{
					for(Declaration*& d:data){
						proto->addElement(d->variable->pointer->name, d->checkTypes(r),filePos);
					}
					//TODO allow default in constructor
				}
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
			functionClass, doubleClass, complexClass, intClass, /*stringClass,*/ charClass,
			sliceClass, voidClass,
			c_stringClass,c_intClass, c_longClass, c_long_longClass, c_pointerClass,byteClass};
	for(ClassProto*& p:cl){
		LANG_M->addPointer(PositionID(0,0,"oclass#init"), p->name, DATA::getClass(p));
	}
}

#endif /* OCLASS_HPP_ */
