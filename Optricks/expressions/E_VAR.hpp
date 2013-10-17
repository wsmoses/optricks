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
		String getFullName() override final{
			return pointer->name;
		}
		const Token getToken() const override{
			return T_VAR;
		}
		ClassProto* getSelfClass() override final{
			return pointer->resolve()->llvmObject.getMyClass();
		}
		Constant* getConstant(RData& a) override final {
			return NULL;
		}
		ReferenceElement* getMetadata(RData& r) final override{
			return pointer->resolve();
		}
		Statement* simplify() override final{
			return this;
		}
		DATA evaluate(RData& r) override final{
			return pointer->resolve()->llvmObject;
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
		ClassProto* checkTypes(RData& r) override{
			ClassProto* temp = pointer->resolve()->returnClass;
			//cout << "LOCATION OF VARIABLE: " << (size_t)(pointer->resolve()) << endl << flush;
			if(temp==NULL) error("Cannot determine return-type of variable "+pointer->name);
			return returnType = temp;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals){
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
