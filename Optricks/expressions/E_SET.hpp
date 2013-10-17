/*
 * E_SET.hpp
 *
 *  Created on: Aug 23, 2013
 *      Author: wmoses
 */

#ifndef E_SET_HPP_
#define E_SET_HPP_

#include "../constructs/Statement.hpp"
#include "./E_VAR.hpp"

class E_SET: public Statement{
	public:
		Statement* variable;
		Statement* value;
		virtual ~E_SET(){}
		E_SET(PositionID id, Statement* loc, Statement* e) : Statement(id){
			variable = loc;
			value = e;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
		}
		const Token getToken() const final override{
			return T_SET;
		}
		Constant* getConstant(RData& a) override final {
			return NULL;
		}
		ClassProto* getSelfClass() override final{ error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
		String getFullName() override final{
			error("Cannot get full name of set");
			return "";
		}
		ClassProto* checkTypes(RData& r) final override{
			returnType = variable->checkTypes(r);
			value->checkTypes(r);
			if(!( variable->returnType == autoClass || value->returnType->hasCast(returnType)))
				error("E_SET of inconsistent types");
			return returnType;
		}

		void registerClasses(RData& r) override final{
			variable->registerClasses(r);
			if(value!=NULL) value->registerClasses(r);
		}
		void registerFunctionArgs(RData& r) override final{
			variable->registerFunctionArgs(r);
			value->registerFunctionArgs(r);
		};
		void registerFunctionDefaultArgs() override final{
			variable->registerFunctionDefaultArgs();
			if(value!=NULL) value->registerFunctionDefaultArgs();
		};
		void resolvePointers() override final{
			variable->resolvePointers();
			if(value!=NULL) value->resolvePointers();
		};
		DATA evaluate(RData& r) final override{
			Value* nex = value->returnType->castTo(r, value->evaluate(r), variable->returnType);
			Value* aloc = variable->getLocation(r);
	//		if(auto t = dynamic_cast<E_LOOKUP*>(variable)){
		//		cout << "IS LOOKUP " << t << endl << flush;
			//} else cout << "Not lookup " << variable->getToken() << endl << flush ;
			if(aloc==NULL) error("Cannot set variable of non-alloc");
			r.builder.CreateStore(nex, aloc);
			return DATA::getConstant(nex);
		}
		E_SET* simplify() final override{
			return new E_SET(filePos, variable,(value->simplify()) );
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_set");
			return NULL;
		}
		void write(ostream& f, String s="") const final override{
			//f << "SET(";
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
			//f << ")";
		}
};

#endif /* E_SET_HPP_ */
