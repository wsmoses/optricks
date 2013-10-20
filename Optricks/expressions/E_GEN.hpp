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
			ClassProto* myClass = prototype->getGeneratorType(filePos);
			myClass->addFunction("iterator", filePos)->funcs.add(DATA::getGenerator(this, new FunctionProto("")), filePos);
			//TODO allow closure for iterator types
			if(innerName.length()==0){
				ReferenceElement* re = self->getMetadata(rd);
				re->returnClass = myClass;
				re->llvmObject = DATA::getGenerator(this, prototype);
				re->funcs.add(re->llvmObject, id);
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
			registerFunctionArgs(r);
			if(innerName.length()>0) return self->getSelfClass()->getFunction(innerName);
			return self->getMetadata(r);
		}
		ClassProto* getSelfClass() override final{
			return prototype->getGeneratorType(filePos);
		}
		const Token getToken() const{
			return T_GEN;
		}
		Constant* getConstant(RData& r) override final{
			return NULL;
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
		void registerFunctionArgs(RData& ra) override{
			if(returnClass!=NULL) returnClass->registerFunctionArgs(ra);
			prototype->returnType = returnClass->getSelfClass();
			self->registerFunctionArgs(ra);
			for(auto& a:prototype->declarations) a->registerFunctionArgs(ra);
			if(innerName.length()>0){
				assert(thisPointer!=NULL);
				thisPointer->returnClass = self->getSelfClass();
				thisPointer->returnClass->addFunction(innerName, filePos)->funcs.add(DATA::getGenerator(this, prototype), filePos);
			}
			ret->registerFunctionArgs(ra);
			ret->registerFunctionDefaultArgs();
			checkTypes(ra);
		};
		void registerFunctionDefaultArgs() override final{
			if(returnClass!=NULL) returnClass->registerFunctionDefaultArgs();
			for(auto& a:prototype->declarations) a->registerFunctionDefaultArgs();
			self->registerFunctionDefaultArgs();
			ret->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			if(returnClass!=NULL) returnClass->resolvePointers();
			for(auto& a:prototype->declarations) a->resolvePointers();
			self->resolvePointers();
			ret->resolvePointers();
		};
		ClassProto* checkTypes(RData& r) override{
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
			return voidClass;
		}
		DATA evaluate(RData& ra) override{
			return DATA::getGenerator(this,prototype);
		}
};


#endif /* E_GEN_HPP_ */
