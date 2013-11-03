/*
 * oobjectproto.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef OOBJECTPROTO_HPP_
#define OOBJECTPROTO_HPP_

#include "../constructs/Statement.hpp"

#define OOBJECT_P_
class oobject: public Statement{
	public:
		oobject(PositionID a, ClassProto* cl):Statement(a, cl){}
		virtual ~oobject(){};
		ReferenceElement* getMetadata(RData& r) override{
			error("Cannot getMetadata() for oobject");
			return NULL;
		}
		oobject* simplify() override final{
			return this;
		}
		void collectReturns(RData& r, std::vector<ClassProto*>& vals,ClassProto* toBe) override final{
		}
		String getFullName() override{
			return returnType->name;
		}

		void registerClasses(RData& r) override{};
		void registerFunctionPrototype(RData& r) override{};
		void buildFunction(RData& r) override{};
		const Token getToken() const override{
			return T_OOBJECT;
		}
		ClassProto* checkTypes(RData& r) override{
			return returnType;
		}
};

/*
		class onull : public oobject{
			public:
				onull():oobject(nullClass){

				}
				Value* evaluate(RData& a){

				}
				operator String() const{
					return "null";
				}
		};

		onull* NULLV = new onull();
 */

#endif /* OOBJECTPROTO_HPP_ */
