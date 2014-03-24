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
		AbstractClass* myClass;
		virtual ~E_TUPLE(){};
		E_TUPLE(const std::vector<Statement*>& a) :Statement(),values(a),myClass(NULL) { };
		const  Token getToken() const override{
			return T_TUPLE;
		};
		/*String getFullName() override{
			if(myClass!=NULL) return myClass->name;
			String s="(";
			bool first = true;
			for(auto& a:values){
				if(first){
					first = false;
				} else s+=",";
				s+=a->getFullName();
			}
			return s+")";
		}*/
		const Data* evaluate(RData& m) const override {
			std::vector<const Data*> vec(values.size());
			for(unsigned i=0; i<values.size(); i++)
				vec[i] = values[i]->evaluate(m);
			return new TupleData(vec);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override final{
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

		AbstractClass* getSelfClass(PositionID id) override final{
			std::vector<const AbstractClass*> vec;
			for(unsigned int i = 0; i<values.size(); i++){
				vec.push_back(values[i]->getSelfClass(id));
			}
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
				for(; i<values.size(); i++){
					auto tmp = values[i]->getReturnType();
					vec.push_back(tmp);
				}
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
		const  Token getToken() const override{
			return T_NAMED_TUPLE;
		};
		const AbstractClass* getReturnType() const override final{
			return &classClass;
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args)const override{
			id.error("Tuple-literal cannot act as function");
			exit(1);
		}
		const AbstractClass* evaluate(RData& m) const override {
			std::vector<const AbstractClass*> vec;
			for(unsigned int i = 0; i<values.size(); i++){
				vec.push_back(values[i]->getSelfClass(filePos));
			}
			return NamedTupleClass::get(vec,names);
		}
		AbstractClass* getSelfClass(PositionID id) override final{
			std::vector<const AbstractClass*> vec;
			for(unsigned int i = 0; i<values.size(); i++){
				vec.push_back(values[i]->getSelfClass(id));
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
