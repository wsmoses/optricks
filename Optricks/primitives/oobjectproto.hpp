/*
 * oobjectproto.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef OOBJECTPROTO_HPP_
#define OOBJECTPROTO_HPP_

#include "../containers/settings.hpp"
#include "../constructs/Statement.hpp"

#ifndef OCLASS_P_
#define OCLASS_P_
class ClassProto;
#endif

#define OOBJECT_P_
class oobject: public Statement{
	public:
		oobject(PositionID a, ClassProto* cl):Statement(a, cl){}
		FunctionProto* getFunctionProto() override{ return NULL; }
		AllocaInst* getAlloc() override final{ return NULL; };
		virtual operator String () const = 0;

		//TODO
		void registerClasses(RData& r) override final{};
		void registerFunctionArgs(RData& r) override{};
		void registerFunctionDefaultArgs() override{};
		void resolvePointers() override{};
		void write(ostream& a, String b) const override;
		oobject* simplify() override final{
			return this;
		}
		const Token getToken() const override{
			return T_OOBJECT;
		}
		ClassProto* checkTypes() override{
			return returnType;
		}
};

		void oobject::write(ostream& a, String b) const{
			a<< (String)(*this);
		}
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
