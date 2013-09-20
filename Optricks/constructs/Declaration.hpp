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
		const Token getToken() const final override{
			return T_DECLARATION;
		}
		ClassProto* checkTypes() final override{
			classV->checkTypes();
			variable->checkTypes();
			if(value!=NULL){
				value->checkTypes();
				if(value->returnType==NULL)error("Declaration of inconsistent types");
				else if(classV->getMetadata()->selfClass==autoClass){
					variable->pointer->resolve()->returnClass = variable->returnType = value->returnType;
					variable->checkTypes();
				}
				else if(!value->returnType->hasCast(classV->getMetadata()->selfClass) )
					error("Declaration of inconsistent types - variable of type "+classV->getMetadata()->name+" and value of "+value->returnType->name);
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
			classV->checkTypes();
			if(classV->getMetadata()->selfClass==NULL) error("Argument " + classV->getMetadata()->name + "is not a class DC");
			variable->getMetadata()->returnClass = variable->returnType = classV->getMetadata()->selfClass;
			variable->checkTypes();
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
			variable->getMetadata()->llvmLocation = Alloca;
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


#endif /* Declaration_HPP_ */
