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
class oclass: public Stackable
{
	public:
		/*oclass* simplify() override final{
			return this;
		}*/
		ClassProto* proto;
		//OModule* module;
		virtual ~oclass(){};
		oclass(ClassProto* p){
			proto = p;
			if(p->name!=""){
				LANG_M->addPointer(PositionID(0,0,"oclass#init"), p->name, NULL,classClass, proto, 0U);
			} else cerr << "String this has no name " << p->name << endl << flush;
			//TODO redo oclass as oobject;
		}

		void write(ostream& ss, String b) const override{
			ss << "class<" + proto->name + ">";
		};
		const Token getToken() const override final {
			return T_OOBJECT;//TODO change to oclass?
		}

		Constant* getConstant(RData& r){
			return NULL;
		}
};

oclass* autoClassV;
void initClasses(){
	//classClass->module = new OModule(objectClass->module);
	initClassesMeta();

	new oclass(classClass);
	new oclass(objectClass);
	autoClassV = new oclass(autoClass);
	//oclass* nullClassO =
	//		new oclass(nullClass);
	//oclass* boolClassO =
			new oclass(boolClass);
	//oclass* arrayClassO =
			//TODO
//			new oclass(arrayClass);
	//oclass* functionClassO =
			new oclass(functionClass);
			new oclass(doubleClass);
			new oclass(complexClass);
	//oclass* intClassO =
			new oclass(intClass);
	//oclass* stringClassO =
			new oclass(stringClass);
			new oclass(charClass);
	//oclass* sliceClassO =
			new oclass(sliceClass);
	//oclass* voidClassO =
			new oclass(voidClass);
			new oclass(c_stringClass);
			new oclass(c_intClass);
			new oclass(c_longClass);
			new oclass(c_long_longClass);

	LANG_M->getPointer(PositionID(0,0,"oclass#init"), "class")->returnClass = classClass;
	LANG_M->getPointer(PositionID(0,0,"oclass#init"), "object")->returnClass = classClass;
}

#endif /* OCLASS_HPP_ */
