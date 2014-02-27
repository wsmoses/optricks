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
class E_ARR : public Statement{
	public:
		std::vector<Statement*> values;
		virtual ~E_ARR(){};
		E_ARR(bool b) : Statement(),values() { };
		E_ARR(const std::vector<Statement*>& a) : Statement(),values(a) { };
		const  Token getToken() const override{
			return T_ARR;
		};
		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<Evaluatable*>& args)const{
							id.error("Array cannot act as function");
							exit(1);
						}
		void write(ostream& f,String a="") const override{
			f << "[";
			for(unsigned int i = 0; i<values.size(); ++i){
				f << values[i];
				if(i<values.size()-1) f << ", ";
			}
			f << "]";
		}
		const Data* evaluate(RData& a) const override {
			//TODO
			PositionID("#arr",0,0).error("E_ARR not implemented");
			/*
			oarray* n = new oarray();
			for(Statement* a: values){
				n->data.push_back(a->evaluate(a));
			}
			return n;*/
			exit(1);
//			return DATA::getConstant(NULL,sliceClass);
		}

		Statement* simplify() override{
			std::vector<Statement*> vals;
			for(Statement*& a: values){
				a = a->simplify();
			}
			return this;
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
			const AbstractClass* min=nullptr;
			for(auto& a:values){
				if(min==nullptr) min=a->getReturnType();
				min = min->getMinClass(a->getReturnType());
			}
			return ArrayClass::get(min, values.size());
		}
};



#endif /* E_ARR_HPP_ */
