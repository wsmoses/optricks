/*
 * E_VAR.hpp
 *
 *  Created on: Apr 13, 2013
 *      Author: wmoses
 */

#ifndef E_VAR_HPP_
#define E_VAR_HPP_

#include "../constructs/Statement.hpp"
#include "../constructs/Module.hpp"

class E_VAR : public Statement{
	public:
		Resolvable* pointer;
		//	Resolvable* type;
		virtual ~E_VAR(){};
		E_VAR(PositionID id, Resolvable* a) : Statement(id),pointer(a){
			//	type = ty;
		};
		const Token getToken() const override{
			return T_VAR;
		}
		ReferenceElement* getMetadata() final override{
			return pointer->resolve();
		}
		Statement* simplify() override final{
			return this;
		}
		DATA evaluate(RData& r) override final{
			auto ans = pointer->resolve()->getValue(r);
			if(ans==NULL) error("No data stored for "+pointer->name);
			return ans;
		}
		void write(ostream& f,String t="") const override{
			f  << pointer->name;
//			f << "v'" << pointer->name << "'";
		}

		void registerClasses(RData& r) override final{
			//TODO
		}
		void registerFunctionArgs(RData& r) override final{
			//TODO
		};
		void registerFunctionDefaultArgs() override final{
			//TODO
		};
		ClassProto* checkTypes() override{
			ClassProto* temp = pointer->resolve()->returnClass;
			if(temp==NULL) error("Cannot determine return-type of variable "+pointer->name);
			return returnType = temp;
		}
		void resolvePointers() override{
			LateResolve* r = dynamic_cast<LateResolve*>(pointer);
			if(r!=NULL){
				pointer = r->resolve();
			}
			if(pointer == NULL) error("What?? Pointer is null? for " + pointer->name);
		}
};
#endif /* E_VAR_HPP_ */
