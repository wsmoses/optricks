/*
 * oclass.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OCLASS_HPP_
#define OCLASS_HPP_

#include "oobjectproto.hpp"

#include "../containers/settings.hpp"

#define OCLASS_P_
class oclass: public oobject{
	public:
		String name;
		Module* module;
		oclass(oclass* init,String nam="");

		operator String () const override{
			return "class<" + name + ">";
		};

		void write(ostream& a, String b) const override final {
			a<< (String)(*this);
		}
		const Token getToken() const override final {
			return T_OOBJECT;
		}
};

oclass* classClass = new oclass(NULL);

oclass::oclass(oclass* init,String nam): oobject((classClass==NULL)?this:classClass){
			if(classClass==NULL) classClass = this;
			name = nam;
			if(init!=NULL)
			module =new Module(init->module);
};

oclass* objectClass = new oclass(NULL,"object");
oclass* nullClass = new oclass(objectClass,"None");
oclass* boolClass = new oclass(objectClass,"bool");
oclass* arrayClass = new oclass(objectClass,"array");
oclass* functionClass = new oclass(objectClass,"function");
oclass* intClass = new oclass(objectClass,"int");
oclass* decClass = new oclass(objectClass,"double");
oclass* stringClass = new oclass(objectClass,"string");
oclass* sliceClass = new oclass(objectClass,"slice");

void initClasses(){
	classClass->module->super = objectClass->module;
	LANG_M->addPointer("class",classClass,0);
	LANG_M->addPointer("object",objectClass,0);
	LANG_M->addPointer("bool",boolClass,0);
	LANG_M->addPointer("array",arrayClass,0);
	LANG_M->addPointer("function",functionClass,0);
	LANG_M->addPointer("int",intClass,0);
	LANG_M->addPointer("double",decClass,0);
	LANG_M->addPointer("string",stringClass,0);
	LANG_M->addPointer("slice",sliceClass,0);
}

#endif /* OCLASS_HPP_ */
