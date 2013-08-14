/*
 * oint.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OINT_HPP_
#define OINT_HPP_

#include "oobjectproto.hpp"
#include <string>
class oint : public oobject{
	public:
		int value;
		oint(int i): oobject(intClass), value(i){}
		operator int& (){
			return value;
		}
		operator String () const override final{
			std::stringstream convert;
			convert << value;
			return convert.str();
		}
		Value* evaluate(RData& a,LLVMContext& context) override final{
			return ConstantInt::get(IntegerType::get(context, 64), value, true);
		}
};


#include "odec.hpp"
#include "ostring.hpp"

/*oobject* operator + (oobject* a) override final;
		oobject* operator + () override final{
			return this;
		}
		oobject* operator - () override final{
			return new oint(-value);
		}
		oobject* operator ~ () override final{
			return new oint(~value);
		}
		oobject* operator += (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot increment int with non-int" << endl;
				exit(0);
			}
			value+=((oint*)a)->value;
			return this;
		}
		oobject* operator - (oobject* a) override final;
		oobject* operator -= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot decrement int with non-int" << endl;
				exit(0);
			}
			value-=((oint*)a)->value;
			return this;
		}
		oobject* operator * (oobject* a) override final;
		oobject* operator *= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot *= int with non-int" << endl;
				exit(0);
			}
			value*=((oint*)a)->value;
			return this;
		}
		oobject* operator / (oobject* a);
		oobject* operator /= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot increment int with non-int" << endl;
				exit(0);
			}
			value/=((oint*)a)->value;
			return this;
		}
		oobject* operator ^ (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot xor (^) int with non-int" << endl;
				exit(0);
			}
			return new oint(value ^ ((oint*)a)->value);
		}
		oobject* operator ^= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot xor (^) int with non-int" << endl;
				exit(0);
			}
			value^=((oint*)a)->value;
			return this;
		}
		oobject* operator % (oobject* a) override final;
		oobject* operator %= (oobject* a){
			if(a->returnType!=intClass){
				cerr << "Cannot modulus = int with non-int" << endl;
				exit(0);
			}
			value%=((oint*)a)->value;
			return this;
		}
		obool* operator != (oobject* a) override final;
		obool* operator == (oobject* a) override final;
		obool* operator < (oobject* a) override final;
		obool* operator <= (oobject* a) override final;
		obool* operator > (oobject* a) override final;
		obool* operator >= (oobject* a) override final;
		oobject* operator | (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot or (|) int with non-int" << endl;
				exit(0);
			}
			return new oint(value | ((oint*)a)->value);
		}
		oobject* operator |= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot or (|) int with non-int" << endl;
				exit(0);
			}
			value|=((oint*)a)->value;
			return this;
		}
		oobject* operator & (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot and (&) int with non-int" << endl;
				exit(0);
			}
			return new oint(value & ((oint*)a)->value);
		}
		oobject* operator &= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot and (&) int with non-int" << endl;
				exit(0);
			}
			value&=((oint*)a)->value;
			return this;
		}
		oobject* operator = (oobject* a) override final{

			if(a->returnType!=intClass){
				cerr << "Cannot = int with non-int" << endl;
				exit(0);
			}
			value=((oint*)a)->value;
			return this;
		}
		oobject* operator << (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot << int with non-int" << endl;
				exit(0);
			}
			return new oint(value << ((oint*)a)->value);
		}
		oobject* operator <<= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot  <<= int with non-int" << endl;
				exit(0);
			}
			value<<=((oint*)a)->value;
			return this;
		}
		oobject* operator >> (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot or >> int with non-int" << endl;
				exit(0);
			}
			return new oint(value | ((oint*)a)->value);
		}
		oobject* operator >>= (oobject* a) override final{
			if(a->returnType!=intClass){
				cerr << "Cannot or (|) int with non-int" << endl;
				exit(0);
			}
			value|=((oint*)a)->value;
			return this;
		}

oobject* oint::operator + (oobject* a){
	if(a->returnType== intClass)
				return new oint(value+((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(value+((odec*)a)->value);
	if(a->returnType==  stringClass)
				return new ostring(
							(String)(*this)
						+((ostring*)a)->value);

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);

}
oobject* oint::operator - (oobject* a){
	if(a->returnType== intClass)
				return new oint(value-((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(value-((odec*)a)->value);

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);

}

oobject* oint::operator * (oobject* a){
	if(a->returnType== intClass)
				return new oint(value*((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(value*((odec*)a)->value);

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);
}

oobject* oint::operator / (oobject* a){
	if(a->returnType== intClass)
				return new oint(value/((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(value/((odec*)a)->value);

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);
}

oobject* oint::operator % (oobject* a){
	if(a->returnType== intClass)
				return new oint(value%((oint*)a)->value);
	if(a->returnType==  decClass)
				return new odec(fmod((double)value,((odec*)a)->value));

			cerr << "Cannot add types int and " << a->returnType;
			exit(1);

}
obool* oint::operator == (oobject* s){
			return new obool( ( intClass==(s)->returnType && ((oint*)s)->value==value)
							|| (decClass==(s)->returnType && ((odec*)s)->value==value));
}

obool* oint::operator != (oobject* s){
	return new obool( ( intClass==(s)->returnType && ((oint*)s)->value!=value)
					|| (decClass==(s)->returnType && ((odec*)s)->value!=value));
}
obool* oint::operator < (oobject* s){
	return new obool( ( intClass==(s)->returnType && ((oint*)s)->value<value)
					|| (decClass==(s)->returnType && ((odec*)s)->value<value));
}

obool* oint::operator <= (oobject* s){
	return new obool( ( intClass==(s)->returnType && ((oint*)s)->value<=value)
					|| (decClass==(s)->returnType && ((odec*)s)->value<=value));
}

obool* oint::operator > (oobject* s){
	return new obool( ( intClass==(s)->returnType && ((oint*)s)->value>value)
					|| (decClass==(s)->returnType && ((odec*)s)->value>value));
}

obool* oint::operator >= (oobject* s){
	return new obool( ( intClass==(s)->returnType && ((oint*)s)->value>=value)
					|| (decClass==(s)->returnType && ((odec*)s)->value>=value));
}
*/
#endif /* OINT_HPP_ */
