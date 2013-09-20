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
		ReferenceElement* getMetadata(){
			error("Cannot getMetadata() for oobject");
			return NULL;
		}
		//TODO
		void registerClasses(RData& r) override final{};
		void registerFunctionArgs(RData& r) override{};
		void registerFunctionDefaultArgs() override{};
		void resolvePointers() override{};
		const Token getToken() const override{
			return T_OOBJECT;
		}
		ClassProto* checkTypes() override{
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
