/*
 * E_GEN.hpp
 *
 *  Created on: Oct 18, 2013
 *      Author: wmoses
 */

#ifndef E_GEN_HPP_
#define E_GEN_HPP_

#include "../../language/statement/Statement.hpp"
#include "./E_FUNCTION.hpp"
#define E_GEN_C_
class E_GEN : public E_FUNCTION{
public:
	VariableReference* self;
//FunctionProto* prototype;
	Statement* body;
	VariableReference* returnClass;
	Resolvable thisPointer;
	mutable bool registereD, builtF;
	AbstractClass* myClass;
	GeneratorFunction generatorFunction;
	AbstractClass* returnType;
	E_GEN(PositionID id, std::vector<Declaration*>& dec, VariableReference* s, VariableReference* ren, Statement* r,Resolvable thi=Resolvable(NULL,"",PositionID(0,0,"<#e_gen.default>"))):
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
			Data* tmp = DATA::getGenerator(this, prototype);
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

	void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		//toLoop->collectReturns(r, vals);
	}
	E_GEN* simplify() override final{
		return this;
	}
	const Token getToken() const{
		return T_GEN;
	}
	void write(ostream& f, String b) const override{
		f << "gen ";
		//f << prototype->returnType->name << " ";
		if(returnClass) f << returnClass;
		f << "(" ;
		bool first = true;
		for(auto &a: declaration){
			if(first) first = false;
			else f << ", " ;
			a->write(f,"");
		}
		f << ")";
		body->write(f, b+"  ");
	}
	void registerFunctionPrototype(RData& ra) const override{
		if(registereD) return;
		registereD = true;
		if(returnClass) returnClass->registerFunctionPrototype(ra);
		self->registerFunctionPrototype(ra);
		for(auto& a:declaration) a->registerFunctionPrototype(ra);
		//for(auto& a:prototype->declarations) a->checkTypes();
		if(thisPointer.name.length()>0){
			const AbstractClass* sC = self->getSelfClass(filePos);
			thisPointer.setObject(new ConstantData(nullptr,sC));
			sC->addFunction(innerName, filePos)->add((SingleFunction*)myFunction, filePos);
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


#endif /* E_GEN_HPP_ */
