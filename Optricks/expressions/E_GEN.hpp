/*
 * E_GEN.hpp
 *
 *  Created on: Oct 18, 2013
 *      Author: wmoses
 */

#ifndef E_GEN_HPP_
#define E_GEN_HPP_

#include "../constructs/Statement.hpp"
#define E_GEN_C_
class E_GEN : public Statement{
	public:
		Statement* self;
		FunctionProto* prototype;
		Statement* ret;
		String innerName;
		Statement* returnClass;
		ReferenceElement* thisPointer;
		E_GEN(PositionID id, Statement* s, Statement* ren, std::vector<Declaration*> dec, Statement* r,RData& rd, String inner="",ReferenceElement* thi=NULL):
			Statement(id, voidClass),
			self(s),
			prototype(new FunctionProto("",dec,NULL)),
			ret(r),
			innerName(inner),
			returnClass(ren),
			thisPointer(thi){
			prototype->name = getFullName();
			ClassProto* myClass = prototype->getGeneratorType(rd, id);
			myClass->addFunction("iterator", filePos)->funcs.add(DATA::getGenerator(this, new FunctionProto("")), rd, filePos);
			//TODO allow closure for iterator types
			if(innerName.length()==0){
				ReferenceElement* re = self->getMetadata(rd);
				re->returnClass = myClass;
				re->llvmObject = DATA::getGenerator(this, prototype);
				re->funcs.add(re->llvmObject, rd, id);
			}
		}
		void registerClasses(RData& r) override final{
			if(returnClass!=NULL) returnClass->registerClasses(r);
			self->registerClasses(r);
			for(auto& a:prototype->declarations) a->registerClasses(r);
			ret->registerClasses(r);
		}

		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
			//toLoop->collectReturns(r, vals);
		}
		E_GEN* simplify() override final{
			return this;
		}
		String getFullName() override final{
			if(innerName.length()>0) return self->getFullName()+"."+innerName;
			return self->getFullName();
		}
		ReferenceElement* getMetadata(RData& r) override final{
			registerFunctionPrototype(r);
			if(innerName.length()>0) return self->getSelfClass(r)->getFunction(innerName, filePos);
			return self->getMetadata(r);
		}
		ClassProto* getSelfClass(RData& r) override final{
			return prototype->getGeneratorType(r, filePos);
		}
		const Token getToken() const{
			return T_GEN;
		}
		void write(ostream& f, String b) const override{
			f << "gen ";
			f << (prototype->name);
			f << "(" ;
			bool first = true;
			for(auto &a: prototype->declarations){
				if(first) first = false;
				else f << ", " ;
				a->write(f,"");
			}
			f << ")";
			if(ret!=NULL) ret->write(f, b);
		}
		void registerFunctionPrototype(RData& ra) override{
			if(returnClass!=NULL) returnClass->registerFunctionPrototype(ra);
			prototype->returnType = returnClass->getSelfClass(ra);
			self->registerFunctionPrototype(ra);
			for(auto& a:prototype->declarations) a->registerFunctionPrototype(ra);
			for(auto& a:prototype->declarations) a->checkTypes(ra);
			if(innerName.length()>0){
				assert(thisPointer!=NULL);
				thisPointer->returnClass = self->getSelfClass(ra);
				thisPointer->returnClass->addFunction(innerName, filePos)->funcs.add(DATA::getGenerator(this, prototype), ra, filePos);
			}
			ret->registerFunctionPrototype(ra);
			//checkTypes(ra); TODO...check if this is necessary
		};
		void buildFunction(RData& r) override final{
			if(returnClass!=NULL) returnClass->buildFunction(r);
			for(auto& a:prototype->declarations) a->buildFunction(r);
			self->buildFunction(r);
			ret->buildFunction(r);


			//TODO check if should move
			if(returnClass!=NULL) returnClass->checkTypes(r);
			self->checkTypes(r);
			for(auto& a:prototype->declarations) a->checkTypes(r);
			ret->checkTypes(r);
			std::vector<ClassProto*> cp;
			ret->collectReturns(r, cp);
			if(cp.size()>0){
				ClassProto* c = getMin(cp, filePos);
				if(c==voidClass) error("Cannot have void yields");
				else if(prototype->returnType==autoClass) prototype->returnType = c;
				else if(!c->equals(prototype->returnType) && !c->hasCast(prototype->returnType)){
					error("Cannot cast yield min yield-type of "+c->name+" to set type of "+prototype->returnType->name);
				}
				returnClass = new ClassProtoWrapper(c);
			}
		};
		void resolvePointers() override final{
			if(returnClass!=NULL) returnClass->resolvePointers();
			for(auto& a:prototype->declarations) a->resolvePointers();
			self->resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
			return voidClass;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getGenerator(this,prototype);
		}
};


#endif /* E_GEN_HPP_ */
