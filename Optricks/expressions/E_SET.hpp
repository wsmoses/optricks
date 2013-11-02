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
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		const Token getToken() const final override{
			return T_SET;
		}
		ClassProto* getSelfClass(RData& r) override final{ error("Cannot get selfClass of construct "+str<Token>(getToken())); return NULL; }
		String getFullName() override final{
			error("Cannot get full name of set");
			return "";
		}
		ClassProto* checkTypes(RData& r) final override{
			returnType = variable->checkTypes(r);
			value->checkTypes(r);
			return returnType;
		}

		void registerClasses(RData& r) override final{
			variable->registerClasses(r);
			value->registerClasses(r);
		}
		void registerFunctionPrototype(RData& r) override final{
			variable->registerFunctionPrototype(r);
			value->registerFunctionPrototype(r);
		};
		void buildFunction(RData& r) override final{
			variable->buildFunction(r);
			value->buildFunction(r);
		};
		DATA evaluate(RData& r) final override{
			DATA nex = value->evaluate(r).castTo(r, variable->returnType, filePos);
			DATA to = variable->evaluate(r);
			Value* aloc = to.getMyLocation();
			r.builder.CreateStore(nex.getValue(r), aloc);
			return nex.toValue(r);
		}
		E_SET* simplify() final override{
			return new E_SET(filePos, variable,(value->simplify()) );
		}
		ReferenceElement* getMetadata(RData& r) override final{
			error("Cannot getMetadata() for E_set");
			return NULL;
		}
		void write(ostream& f, String s="") const final override{
			variable->write(f);
			if(value!=NULL) {
				f << "=";
				value->write(f);
			}
		}
};

#endif /* E_SET_HPP_ */
