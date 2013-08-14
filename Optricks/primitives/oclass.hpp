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
class oclass
		//: public oobject
{
	public:
		String name;
		std::map<String,std::map<oclass*, obinop*> > binops;
		std::map<String,ouop* > preops;
		std::map<String,ouop* > postops;
		OModule* module;
		oclass* super;
		oclass(oclass* init,String nam=""){
			super = init;
			name = nam;
			//TODO redo oclass as oobject;
		}

		operator String () const{
			return "class<" + name + ">";
		};
/*
		void write(ostream& a, String b) const override final {
			a<< (String)(*this);
		}
		const Token getToken() const override final {
			return T_OOBJECT;
		}*/
};

oclass* classClass = new oclass(NULL);

/*
oclass::oclass(oclass* init,String nam): oobject((classClass==NULL)?this:classClass){
			if(classClass==NULL) classClass = this;
			name = nam;
			if(init!=NULL)
			module =new OModule(init->module);
};*/

oclass* objectClass = new oclass(NULL,"object");
oclass* nullClass = new oclass(objectClass,"None");
oclass* boolClass = new oclass(objectClass,"bool");
oclass* arrayClass = new oclass(objectClass,"array");
oclass* functionClass = new oclass(objectClass,"function");
oclass* decClass = new oclass(objectClass,"double");
oclass* intClass = new oclass(decClass,"int");
oclass* stringClass = new oclass(objectClass,"string");
oclass* sliceClass = new oclass(objectClass,"slice");

void initClasses(){
	//classClass->module = new OModule(objectClass->module);
	classClass->super = objectClass;
	///////******************************* Boolean ********************************////////
	boolClass->binops["&&"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->binops["!="][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpNE(a,b,"andtmp");
	},boolClass);

	boolClass->binops["=="][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpEQ(a,b,"andtmp");
	},boolClass);

	boolClass->binops["||"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);

	boolClass->preops["!"] = new ouopNative(
			[](Value* a, RData& m) -> Value*{
				return m.builder.CreateNot(a,"nottmp");
	},boolClass);

	///////******************************* Double/Double ******************************////////
	decClass->binops["+"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(a,b,"addtmp");
	},decClass);

	decClass->binops["-"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(a,b,"subtmp");
	},decClass);

	decClass->binops["*"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(a,b,"multmp");
	},decClass);

	decClass->binops["%"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(a,b,"modtmp");
	},decClass);

	decClass->binops["<"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops[">"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["<="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops[">="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["=="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["!="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->binops["/"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(a,b,"divtmp");
	},decClass);

	decClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateFNeg(a,"negtmp");
	},decClass);

	decClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},decClass);


	///////******************************* INT ********************************////////
	intClass->binops["+"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateAdd(a,b,"addtmp");
	},intClass);

	intClass->binops["-"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSub(a,b,"subtmp");
	},intClass);

	intClass->binops["*"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateMul(a,b,"multmp");
	},intClass);

	intClass->binops["%"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSRem(a,b,"modtmp");
	},intClass);

	intClass->binops["<"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["<="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["=="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["!="][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["/"][intClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSDiv(a,b,"divtmp");
	},intClass);

	intClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateNeg(a,"negtmp");
	},intClass);

	intClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},intClass);

	///////******************************* INT/Double ********************************////////
	intClass->binops["+"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(m.builder.CreateSIToFP(a,b->getType()),b,"addtmp");
	},decClass);

	intClass->binops["-"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(m.builder.CreateSIToFP(a,b->getType()),b,"subtmp");
	},decClass);

	intClass->binops["*"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(m.builder.CreateSIToFP(a,b->getType()),b,"multmp");
	},decClass);

	intClass->binops["%"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(m.builder.CreateSIToFP(a,b->getType()),b,"modtmp");
	},decClass);

	intClass->binops["<"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["<="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["=="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["!="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["/"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(m.builder.CreateSIToFP(a,b->getType()),b,"divtmp");
	},intClass);
	/*
	LANG_M->addPointer("class",classClass,0);
	LANG_M->addPointer("object",objectClass,0);
	LANG_M->addPointer("bool",boolClass,0);
	LANG_M->addPointer("array",arrayClass,0);
	LANG_M->addPointer("function",functionClass,0);
	LANG_M->addPointer("int",intClass,0);
	LANG_M->addPointer("double",decClass,0);
	LANG_M->addPointer("string",stringClass,0);
	LANG_M->addPointer("slice",sliceClass,0);*/
}

#endif /* OCLASS_HPP_ */
