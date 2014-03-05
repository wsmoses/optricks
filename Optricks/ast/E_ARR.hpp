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
class E_ARR : public ErrorStatement{
	public:
		std::vector<Statement*> values;
		virtual ~E_ARR(){};
		E_ARR(PositionID id, bool b) : ErrorStatement(id),values() { };
		E_ARR(PositionID id, const std::vector<Statement*>& a) : ErrorStatement(id),values(a) { };
		const  Token getToken() const override{
			return T_ARR;
		};
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
			id.error("Array cannot act as function");
			exit(1);
		}
		const Data* evaluate(RData& a) const override {
			//TODO
			filePos.compilerError("E_ARR not implemented");
			/*
			oarray* n = new oarray();
			for(Statement* a: values){
				n->data.push_back(a->evaluate(a));
			}
			return n;*/
			exit(1);
//			return DATA::getConstant(NULL,sliceClass);
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
				//todo
				filePos.compilerError("Literal array of unknown length");
			}
			std::vector<const AbstractClass*> vc;
			for(auto& a: values) vc.push_back(a->getReturnType());
			return ArrayClass::get(getMin(vc,filePos), values.size());
		}
};



#endif /* E_ARR_HPP_ */
