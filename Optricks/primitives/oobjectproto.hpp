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
		virtual oobject* operator + ();
		virtual oobject* operator - ();
		virtual oobject* operator ! ();
		virtual oobject* operator ~ ();
		virtual oobject* operator [] (oobject* a);
		virtual oobject* operator + (oobject* a);
		virtual oobject* operator += (oobject* a);
		virtual oobject* operator - (oobject* a);
		virtual oobject* operator -= (oobject* a);
		virtual oobject* operator * (oobject* a);
		virtual oobject* operator *= (oobject* a);
		virtual oobject* operator / (oobject* a);
		virtual oobject* operator /= (oobject* a);
		virtual oobject* operator ^ (oobject* a);
		virtual oobject* operator ^= (oobject* a);
		virtual oobject* operator % (oobject* a);
		virtual oobject* operator %= (oobject* a);
		virtual obool* operator != (oobject* a);
		virtual obool* operator == (oobject* a);
		virtual obool* operator < (oobject* a);
		virtual obool* operator <= (oobject* a);
		virtual obool* operator > (oobject* a);
		virtual obool* operator >= (oobject* a);
		virtual oobject* operator | (oobject* a);
		virtual oobject* operator |= (oobject* a);
		virtual obool* operator || (oobject* a);
		virtual oobject* operator & (oobject* a);
		virtual oobject* operator &= (oobject* a);
		virtual obool* operator && (oobject* a);
		virtual oobject* operator = (oobject* a);
		virtual oobject* operator << (oobject* a);
		virtual oobject* operator >> (oobject* a);
		virtual oobject* operator <<= (oobject* a);
		virtual oobject* operator >>= (oobject* a);
		oobject* evaluate() override final;
		oobject* simplify() override final;
		const Token getToken() const override{
			return T_OOBJECT;
		}
};

#include "oclass.hpp"
#include "obool.hpp"


		void oobject::write(ostream& a, String b) const{
			a<< (String)(*this);
		}
		oobject* oobject::evaluate(){
			return this;
		}
		oobject* oobject::simplify(){
			return this;
		}


		class onull : public oobject{
			public:
				onull();
				obool* operator == (oobject* s) override{
					return new obool(nullClass==s->returnType);
				}
				operator String () const override;
		};

		onull* NULLV = new onull();

		onull::onull(): oobject(nullClass){};

		onull::operator String () const{
			return "null";
		}

#include "ostring.hpp"



		oobject* oobject::operator + (){
			cerr << "Unary Operation + not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator - (){
			cerr << "Unary Operation - not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator ! (){
			cerr << "Unary Operation ! not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator ~ (){
			cerr << "Unary Operation ~ not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator [] (oobject* a){
			cerr << "Lookup Operation [] not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator + (oobject* a){
			if(a->returnType==stringClass) return new ostring((String)(*this)+((ostring*)a)->value);
			cerr << "Operation [] not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator += (oobject* a){
			cerr << "Binary Operation += not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator - (oobject* a){
			cerr << "Binary Operation - not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator -= (oobject* a){
			cerr << "Binary Operation -= not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator * (oobject* a){
			cerr << "Binary Operation * not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator *= (oobject* a){
			cerr << "Binary Operation *= not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator / (oobject* a){
			cerr << "Binary Operation / not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator /= (oobject* a){
			cerr << "Binary Operation /= not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator ^ (oobject* a){
			cerr << "Binary Operation ^ not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator ^= (oobject* a){
			cerr << "Binary Operation ^= not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator % (oobject* a){
			cerr << "Binary Operation % not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator %= (oobject* a){
			cerr << "Binary Operation %= not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator != (oobject* a){
			cerr << "Binary Operation != not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator == (oobject* a){
			cerr << "Binary Operation == not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator < (oobject* a){
			cerr << "Binary Operation < not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator <= (oobject* a){
			cerr << "Binary Operation ^ not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator > (oobject* a){
			cerr << "Binary Operation > not permitted" << endl;
			exit(0);
		}
		obool* oobject::operator >= (oobject* a){
			cerr << "Binary Operation >= not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator | (oobject* a){
			cerr << "Binary Operation | not permitted" << endl;
			exit(0);
		}
		oobject* oobject::operator |= (oobject* a){
			cerr << "Binary Operation |= not permitted" << endl;
			exit(0);
		};
		obool* oobject::operator || (oobject* a){
			cerr << "Binary Operation || not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator & (oobject* a){
			cerr << "Binary Operation & not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator &= (oobject* a){
			cerr << "Binary Operation &= not permitted" << endl;
			exit(0);
		};
		obool* oobject::operator && (oobject* a){
			cerr << "Binary Operation && not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator = (oobject* a){
			cerr << "Binary Operation = not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator << (oobject* a){
			cerr << "Binary Operation << not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator >> (oobject* a){
			cerr << "Binary Operation >> not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator <<= (oobject* a){
			cerr << "Binary Operation <<= not permitted" << endl;
			exit(0);
		};
		oobject* oobject::operator >>= (oobject* a){
			cerr << "Binary Operation >>= not permitted" << endl;
			exit(0);
		};
#endif /* OOBJECTPROTO_HPP_ */
