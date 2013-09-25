/*
 * DECLARATION.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DECLARATION_HPP_
#define DECLARATION_HPP_

#include "../constructs/Statement.hpp"
#include "../expressions/E_VAR.hpp"

#define DECLR_P_
class Declaration: public Construct{
	public:
		Statement* classV;
		E_VAR* variable;
		Statement* value;
		AllocaInst* Alloca;
		Declaration(PositionID id, Statement* v, E_VAR* loc, Statement* e=NULL) : Construct(id, voidClass){
			classV = v;
			variable = loc;
			value = e;
			Alloca = NULL;
		}
		//TODO check
		bool hasValue() const {
			return value!=NULL;
		}
		const Token getToken() const final override{
			return T_DECLARATION;
		}
		ClassProto* checkTypes(RData& r) final override{
			classV->checkTypes(r);
			variable->checkTypes(r);
			if(value!=NULL){
				value->checkTypes(r);
				if(value->returnType==NULL)error("Declaration of inconsistent types");
				else if(classV->getMetadata(r)->selfClass==autoClass){
					variable->pointer->resolve()->returnClass = variable->returnType = value->returnType;
					variable->checkTypes(r);
				}
				else if(!value->returnType->hasCast(classV->getMetadata(r)->selfClass) )
					error("Declaration of inconsistent types - variable of type "+classV->getFullName()+" and value of "+value->returnType->name);
			}
			return returnType;
		}

		void registerClasses(RData& r) override final{
			classV->registerClasses(r);
			variable->registerClasses(r);
			if(value!=NULL) value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			classV->registerFunctionArgs(r);
			variable->registerFunctionArgs(r);
			if(value!=NULL) value->registerFunctionArgs(r);
			classV->checkTypes(r);
			if(classV->getMetadata(r)->selfClass==NULL) error("Argument " + classV->getFullName() + "is not a class DC");
			variable->getMetadata(r)->returnClass = variable->returnType = classV->getMetadata(r)->selfClass;
			variable->checkTypes(r);
			//TODO add name in table of args?
		};
		void registerFunctionDefaultArgs() override final{
			classV->registerFunctionDefaultArgs();
			variable->registerFunctionDefaultArgs();
			if(value!=NULL) value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			classV->resolvePointers();
			variable->resolvePointers();
			if(value!=NULL) value->resolvePointers();
		};
		DATA evaluate(RData& r) final override{
			Function *TheFunction = r.builder.GetInsertBlock()->getParent();
			IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
					TheFunction->getEntryBlock().begin());
			Alloca = TmpB.CreateAlloca(variable->returnType->getType(r), 0,variable->pointer->name);
			if(value!=NULL && value->getToken()!=T_VOID){
				r.builder.CreateStore(value->returnType->castTo(r, value->evaluate(r), variable->returnType) , Alloca);
			}
			variable->getMetadata(r)->llvmLocation = Alloca;
			r.guarenteedReturn = false;
			return NULL;
			//variable->pointer->resolve() = r.builder.CreateLoad(Alloca);
//			error("Todo: allow declaration evaluation");
		}
		Declaration* simplify() final override{
			return new Declaration(filePos, classV, variable, (value==NULL)?NULL:(value->simplify()) );
		}
		void write(ostream& f, String s="") const final override{
			//f << "d(";
			if(classV!=NULL) f << classV << " ";
			else f << "auto ";
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
			//f << ")";
		}
};

std::pair<bool,std::pair<unsigned int, unsigned int>> FunctionProto::match(FunctionProto* func,RData& r) const{
			unsigned int optional;
			if(func->declarations.size()!=declarations.size()){
				if(declarations.size()>func->declarations.size()) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
				for(unsigned int a=declarations.size(); a<func->declarations.size(); ++a){
					if(!func->declarations[a]->hasValue()) return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
				}
				optional = func->declarations.size()-declarations.size();
			} else optional = 0;
			unsigned int count=0;
			for(unsigned int a=0; a<declarations.size(); ++a){
				ClassProto* class1 = declarations[a]->classV->getMetadata(r)->selfClass;
				ClassProto* class2 = func->declarations[a]->classV->getMetadata(r)->selfClass;
				auto t = class1->compatable(class2);
				if(!t.first)  return std::pair<bool,std::pair<unsigned int, unsigned int> >(false,std::pair<unsigned int, unsigned int>(0,0));
				else{
					if(t.second>0) count++;
				}
			}
			return std::pair<bool,std::pair<unsigned int, unsigned int> >(true,std::pair<unsigned int, unsigned int>(optional,count));
		}

bool FunctionProto::equals(const FunctionProto* f,RData& r) const{
	if(declarations.size()!=f->declarations.size()) return false;
	for(unsigned int i = 0; i<declarations.size(); ++i){
		ClassProto* class1 = declarations[i]->classV->getMetadata(r)->selfClass;
		ClassProto* class2 = f->declarations[i]->classV->getMetadata(r)->selfClass;
		if(class1==NULL || class2==NULL) todo("ERROR: NULL PROTO",PositionID());
		if(!class1->equals(class2))
			return false;
	}
	return true;
}
String FunctionProto::toString() const{
	String t = name+"(";
	bool first = true;
	for(const auto& a: declarations){
		if(first){
			first = false;
		} else t+=",";
		t+=a->classV->getFullName();
	}
	return t+")";
}
#endif /* Declaration_HPP_ */
