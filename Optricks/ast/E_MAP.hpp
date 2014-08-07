/*
 * E_MAP.hpp
 *
 *  Created on: Jun 26, 2014
 *      Author: Billy
 */

#ifndef E_MAP_HPP_
#define E_MAP_HPP_

#include "../language/statement/Statement.hpp"
#include "../language/data/MapData.hpp"
class E_MAP : public ErrorStatement{
	public:
		std::vector<std::pair<Statement*,Statement*> > inner;
		virtual ~E_MAP(){};
		E_MAP(PositionID i, const std::vector<Statement*>& a, const std::vector<Statement*>& b) :ErrorStatement(i), inner(a.size()){
			assert(a.size()>0);
			assert(b.size()==a.size());
			for(unsigned i=0; i<a.size(); i++)
				inner[i] = std::pair<Statement*,Statement*>(a[i],b[i]);
		};
		E_MAP(PositionID i, const std::vector<std::pair<Statement*,Statement*> >& a) :ErrorStatement(i),inner(a){
			assert(inner.size()>0);
		};
		const  Token getToken() const override{
			return T_MAP;
		};

		void reset() const override final{
			for(auto& a: inner){
				a.first->reset();
				a.second->reset();
			}
		}
		const Data* evaluate(RData& m) const override {
			std::vector<std::pair<const Data*,const Data*> > vec(inner.size());
			for(unsigned i=0; i<inner.size(); i++)
				vec[i] = std::pair<const Data*,const Data*>(inner[i].first->evaluate(m),inner[i].second->evaluate(m));
			assert(vec.size()==inner.size());
			return new MapData(filePos, vec);
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override final{
			if(inner.size()!=1){
				id.error("Cannot use map as function");
				exit(1);
				return nullptr;
			}
			return HashMapClass::get(inner[0].first->getMyClass(rdata, id), inner[0].second->getMyClass(rdata, id));
		}

		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		void registerClasses() const override final{
			for(auto& a: inner){
				a.first->registerClasses();
				a.second->registerClasses();
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			for(auto& a:inner){
				a.first->registerFunctionPrototype(r);
				a.second->registerFunctionPrototype(r);
			}
		};


		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			if(inner.size()!=1)
				id.error("Cannot use map literal of size!=1 as class");
			return HashMapClass::get(inner[0].first->getMyClass(r, id), inner[0].second->getMyClass(r, id));
		}
		void buildFunction(RData& r) const override final{
			for(auto& a:inner){
				a.first->buildFunction(r);
				a.second->buildFunction(r);
			}
		};
		const AbstractClass* getReturnType() const override{
			assert(inner.size()>0);
			const AbstractClass* A = inner[0].first->getReturnType();
			const AbstractClass* B = inner[0].second->getReturnType();
			if(A->classType==CLASS_CLASS && B->classType==CLASS_CLASS && inner.size()==1) return &classClass;
			for(unsigned i=1; i<inner.size(); i++){
				A = getMin(A, inner[i].first->getReturnType(),filePos);
				B = getMin(B, inner[i].second->getReturnType(),filePos);
			}
			return HashMapClass::get(A, B);
		}
};



#endif /* E_MAP_HPP_ */
