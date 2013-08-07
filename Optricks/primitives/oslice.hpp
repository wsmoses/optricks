/*
 * oslice.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OSLICE_HPP_
#define OSLICE_HPP_

#include "oobjectproto.hpp"

class oslice : public oobject{
	public:
		oobject* start, *stop, *step;
		oslice(oobject* a, oobject* b, oobject* c): oobject(sliceClass),
				start(a), stop(b),step(c){
			if(step==0){
				cerr << "Step in slice cannot be 0";
				exit(0);
			}
		}
		operator String () const override{
			std::stringstream ss;
			ss << "[";
			if(start!=NULLV) ss << start;
			ss << ":";
			if(step!=NULLV) ss << stop;
			ss << ":";
			if(step!=NULLV) ss << step;
			ss << "]";
			return ss.str();
		}
};

class E_SLICE : public Expression{
public:
	Expression* start, *stop, *step;
	E_SLICE(Expression* a, Expression* b, Expression* c): Expression(sliceClass),
			start(a), stop(b),step(c){}
	const Token getToken() const override{
		return T_SLICE;
	};
	oobject* evaluate() override final{
		oobject *a = start->evaluate(),
				*b = stop->evaluate(),
				*c = step->evaluate();
		if( !(a->returnType==intClass ||
				a->returnType==nullClass) ||
				!(b->returnType==intClass ||
				b->returnType==nullClass) ||
				!(c->returnType==intClass ||
				c->returnType==nullClass)){
			cerr << "Cannot make slice out of unknown type " << a->returnType << ":" << b->returnType << ":" << c->returnType;
			exit(0);
		}
		return new oslice(a,b,c);
	}
	Expression* simplify() override{

		Expression *aa = start->evaluate(),
				*bb = stop->evaluate(),
				*cc = step->evaluate();
		if(aa->getToken()!=T_OOBJECT || bb->getToken()!=T_OOBJECT || cc->getToken()!=T_OOBJECT)
			return new E_SLICE(aa,bb,cc);
		oobject *a = (oobject*)aa,
				*b = (oobject*)bb,
				*c = (oobject*)cc;
		if(!(a->returnType==intClass ||
				a->returnType==nullClass) ||
				!(b->returnType==intClass ||
				b->returnType==nullClass) ||
				!(c->returnType==intClass ||
				c->returnType==nullClass)){
			return new E_SLICE(aa,bb,cc);
		}
		return new oslice(a,b,c);
	}
	void write(ostream& f,String a="") const override{
		f << "E_SLICE(";
		start->write(f,"");
		f<<":";
		stop->write(f,"");
		f<<":";
		step->write(f,"");
		f << ")";
	}
};

#endif /* OSLICE_HPP_ */
