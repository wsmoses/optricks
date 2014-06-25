/*
 * E_ARR.hpp
 *
 *  Created on: Jan 30, 2014
 *      Author: Billy
 */

#ifndef E_ARR_HPP_
#define E_ARR_HPP_
#include "../language/statement/Statement.hpp"
#include "../language/class/builtin/ArrayClass.hpp"
#include "../language/data/ArrayData.hpp"
class E_ARR : public ErrorStatement{
	public:
		std::vector<Statement*> values;
		virtual ~E_ARR(){};
		E_ARR(PositionID id, bool b) : ErrorStatement(id),values() { };
		E_ARR(PositionID id, const std::vector<Statement*>& a) : ErrorStatement(id),values(a) { };
		const  Token getToken() const override{
			return T_ARR;
		};

		void reset() const override final{
			for(auto& a: values)
				a->reset();
		}
		const AbstractClass* getMyClass(RData& r, PositionID id)const{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			id.error("Array cannot act as function");
			exit(1);
		}
		const Data* evaluate(RData& a) const override {
			std::vector<const Data*> V(values.size());
			for(unsigned i=0; i<values.size(); i++)
				V[i] = values[i]->evaluate(a)->toValue(a, filePos);
			return new ArrayData(V,filePos);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
		//ClassProto* getSelfClass() override final{ error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
		void registerClasses() const override final{
			for(auto& a:values){
				a->registerClasses();
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			for(auto& a:values){
				a->registerFunctionPrototype(r);
			}
		};
		void buildFunction(RData& r) const override final{
			for(auto& a:values){
				a->buildFunction(r);
			}
		};
		const AbstractClass* getReturnType() const override{
			if(values.size()==0){
				return ArrayClass::get(nullptr,0);
			}
			const AbstractClass* a=values[0]->getReturnType();
			for(unsigned i=0; i<values.size(); i++)
				a = getMin(a, values[i]->getReturnType(), filePos);
			return ArrayClass::get(a, values.size());
		}
};



#endif /* E_ARR_HPP_ */
