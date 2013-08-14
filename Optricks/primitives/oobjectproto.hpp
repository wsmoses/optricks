/*
 * oobjectproto.hpp
 *
 *  Created on: Jul 31, 2013
 *      Author: wmoses
 */

#ifndef OOBJECTPROTO_HPP_
#define OOBJECTPROTO_HPP_

#include "../containers/settings.hpp"
#include "../constructs/Expression.hpp"
#include "../constructs/Module.hpp"

#ifndef OCLASS_P_
#define OCLASS_P_
class oclass;
#endif

#ifndef OBOOL_P_
#define OBOOL_P_
class obool;
#endif

#define OOBJECT_P_
class oobject: public Expression{
	public:
		oobject(oclass* c):Expression(c){}
		virtual operator String () const = 0;

		void write(ostream& a, String b) const override;
		oobject* simplify() override final{
			return this;
		}
		const Token getToken() const override{
			return T_OOBJECT;
		}
};

#include "oclass.hpp"
#include "obool.hpp"


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
