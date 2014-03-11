/*
 * ClassGen.hpp
 *
 *  Created on: Mar 2, 2014
 *      Author: Billy
 */

#ifndef CLASSGEN_HPP_
#define CLASSGEN_HPP_

/*
#include "../../language/statement/Statement.hpp"
#include "./E_FUNCTION.hpp"
#include "../../language/class/ClassLib.hpp"
#include "../../language/location/Location.hpp"
#define ClassGen_C_
class ClassGen : public E_FUNCTION{
public:
	VariableReference* self;
//FunctionProto* prototype;
	Statement* body;
	Statement* returnClass;
	Resolvable thisPointer;
	mutable bool registereD, builtF;
	AbstractClass* myClass;
	GeneratorFunction generatorFunction;
	AbstractClass* returnType;
	ClassGen(PositionID id, std::vector<Declaration*>& dec, VariableReference* s, Statement* ren, Statement* r,Resolvable thi=Resolvable(NULL,"",PositionID(0,0,"<#e_gen.default>"))):
		E_FUNCTION(id,dec),
		self(s),
		//prototype(new FunctionProto("",dec,NULL)),
		body(r),
		returnClass(ren),
		thisPointer(thi){
		registereD = builtF = false;
		//prototype->name = getFullName();
		myClass = prototype->getGeneratorType();
		SingleFunction* sf = new GeneratorFunction(this, new FunctionProto(""));
		myClass->staticVariables.addFunction(filePos,"iterator")->add(sf,filePos);
		//TODO allow closure for iterator types
		if(thisPointer.name.length()==0){
			Resolvable re = self->getMetadata();
			SingleFunction* tmp = DATA::getGenerator(this, prototype);
			re.setObject(tmp);
			//TODO fix this
			re.addFunction(tmp);
		}
		returnType = nullptr;
		assert(body);
	}
	void registerClasses() const override final{
		if(returnClass) returnClass->registerClasses();
		//self->registerClasses(r);
		for(auto& a:declaration) a->registerClasses();
		body->registerClasses();
	}
	const Token getToken() const{
		return T_GEN;
	}
	void registerFunctionPrototype(RData& ra) const override{
		if(registereD) return;
		registereD = true;
		if(returnClass) returnClass->registerFunctionPrototype(ra);
		self->registerFunctionPrototype(ra);
		for(auto& a:declaration) a->registerFunctionPrototype(ra);
		if(thisPointer.name.length()>0){
			const AbstractClass* sC = self->getSelfClass(filePos);
			thisPointer.setObject(new ConstantData(nullptr,sC));
			sC->addLocalFunction((String)innerName)->add((SingleFunction*)myFunction, filePos);
		}
		body->registerFunctionPrototype(ra);

		std::vector<const AbstractClass*> cp;
		const AbstractClass* ret = (returnClass)?(returnClass->getSelfClass(filePos)):(nullptr);
		body->collectReturns(cp,myFunction->getSingleProto()->returnType);
		if(!ret){
			if(cp.size()==0){
				filePos.error("Cannot have auto-returning generator with no yield statements");
				ret=voidClass;
			}
			else{
				const AbstractClass* c = getMin(cp, filePos);
				assert(c);
				if(c->classType==CLASS_VOID) filePos.error("Cannot have void yields");
				ret = c;
			}
		}
		//TODO use ret to build generator
	};
	void buildFunction(RData& r) const override final{
		registerFunctionPrototype(r);
		if(builtF) return;
		builtF = true;
		if(returnClass!=NULL) returnClass->buildFunction(r);
		for(auto& a:declaration) a->buildFunction(r);
		self->buildFunction(r);
		body->buildFunction(r);
	};
};

*/

#endif /* CLASSGEN_HPP_ */
