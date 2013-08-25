/*
 * oclass.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "../containers/settings.hpp"
#include "oobjectproto.hpp"
#include "../constructs/Module.hpp"


#define OCLASS_P_
class oclass: public Stackable
{
	public:
		ClassProto* proto;
		OModule* module;
		oclass* super;
		oclass(oclass* init,ClassProto* p){
			proto = p;
			super = init;
			if(p->name!=""){
				LANG_M->addPointer(PositionID(0,0,"oclass#init"), p->name, (Value*)NULL,classClass, NULL,proto, 0U);
			} else cerr << "String this has no name " << p->name << endl << flush;
			//TODO redo oclass as oobject;
		}

		void write(ostream& ss, String b) const override{
			ss << "class<" + proto->name + ">";
		};
		const Token getToken() const override final {
			return T_OOBJECT;//TODO change to oclass?
		}
};


void initClasses(){
	//classClass->module = new OModule(objectClass->module);
	initClassesMeta();

	oclass* classClassO = new oclass(NULL,classClass);
	oclass* objectClassO = new oclass(NULL,objectClass);
	//oclass* nullClassO =
			new oclass(objectClassO,nullClass);
	//oclass* boolClassO =
			new oclass(objectClassO,boolClass);
	//oclass* arrayClassO =
			new oclass(objectClassO,arrayClass);
	//oclass* functionClassO =
			new oclass(objectClassO,functionClass);
	oclass* decClassO = new oclass(objectClassO,decClass);
	//oclass* intClassO =
			new oclass(decClassO,intClass);
	//oclass* stringClassO =
			new oclass(objectClassO,stringClass);
	//oclass* sliceClassO =
			new oclass(objectClassO,sliceClass);
	//oclass* voidClassO =
			new oclass(objectClassO,voidClass);

	classClassO->super = objectClassO;
	LANG_M->getPointer(PositionID(0,0,"oclass#init"), "class")->resolveReturnClass() = classClass;
	LANG_M->getPointer(PositionID(0,0,"oclass#init"), "object")->resolveReturnClass() = classClass;
}

#endif /* OCLASS_HPP_ */
