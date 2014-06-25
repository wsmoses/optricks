/*
 * E_TUPLE.hpp
 *
 *  Created on: Nov 3, 2013
 *      Author: wmoses
 */

#ifndef E_TUPLE_HPP_
#define E_TUPLE_HPP_

#include "../language/statement/Statement.hpp"
class E_TUPLE : public Statement{
	public:
		std::vector<Statement*> values;
		//AbstractClass* myClass;
		virtual ~E_TUPLE(){};
		E_TUPLE(const std::vector<Statement*>& a) :Statement(),values(a)/*,myClass(NULL)*/ { };
		const  Token getToken() const override{
			return T_TUPLE;
		};

		void reset() const override final{
			for(auto& a: values) a->reset();
		}
		const Data* evaluate(RData& m) const override {
			std::vector<const Data*> vec(values.size());
			for(unsigned i=0; i<values.size(); i++)
				vec[i] = values[i]->evaluate(m);
			assert(vec.size()==values.size());
			return new TupleData(vec);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			id.error("Tuple-literal cannot act as function");
			exit(1);
		}

		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
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


		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			std::vector<const AbstractClass*> vec(values.size());
			for(unsigned int i = 0; i<values.size(); i++){
				vec[i] = values[i]->getMyClass(r, id);
			}
			assert(vec.size()==values.size());
			return TupleClass::get(vec);
		}
		void buildFunction(RData& r) const override final{
			for(auto& a:values){
				a->buildFunction(r);
			}
		};
		const AbstractClass* getReturnType() const override{
			std::vector<const AbstractClass*> vec;
			unsigned int i;
			for(i=0; i<values.size(); i++){
				auto tmp = values[i]->getReturnType();
				vec.push_back(tmp);
				if(tmp->classType!=CLASS_CLASS) break;
			}
			if(i==values.size()) return &classClass;
			else{
				i++;
				for(; i<values.size(); i++){
					auto tmp = values[i]->getReturnType();
					vec.push_back(tmp);
				}
				assert(vec.size()==values.size());
				return TupleClass::get(vec);
			}
		}
};


class E_NAMED_TUPLE : public ErrorStatement{
	public:
		const std::vector<Statement*> values;
		const std::vector<String> names;
		virtual ~E_NAMED_TUPLE(){};
		E_NAMED_TUPLE(PositionID id, const std::vector<Statement*>& a,const std::vector<String>& b) :
			ErrorStatement(id),values(a),names(b){
			assert(a.size()==b.size());
			for(unsigned i=0; i<a.size(); i++){
				assert(b[i].length()>0);
				for(unsigned j=0; j<i; j++)
					assert(b[i]!=b[j]);
			}
		};

		void reset() const override final{
			for(auto& a: values) a->reset();
		}
		const  Token getToken() const override{
			return T_NAMED_TUPLE;
		};
		const AbstractClass* getReturnType() const override final{
			return &classClass;
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			id.error("Tuple-literal cannot act as function");
			exit(1);
		}
		const AbstractClass* evaluate(RData& m) const override {
			std::vector<const AbstractClass*> vec(values.size());
			for(unsigned int i = 0; i<values.size(); i++){
				vec[i] = values[i]->getMyClass(m, filePos);
			}
			assert(vec.size()==values.size());
			return NamedTupleClass::get(vec,names);
		}

		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			std::vector<const AbstractClass*> vec(values.size());
			for(unsigned int i = 0; i<values.size(); i++){
				vec[i] = values[i]->getMyClass(r, id);
			}
			assert(values.size()==vec.size());
			assert(vec.size()==names.size());
			return NamedTupleClass::get(vec,names);
		}
		void collectReturns(std::vector<const AbstractClass*>& vals,const AbstractClass* const toBe) override final{
		}
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
};


#endif /* E_TUPLE_HPP_ */
