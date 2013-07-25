/*
 * DefaultDeclaration.hpp
 *
 *  Created on: Apr 14, 2013
 *      Author: wmoses
 */

#ifndef DEFAULTDECLARATION_HPP_
#define DEFAULTDECLARATION_HPP_

#include "../O_Expression.hpp"
class DefaultDeclaration{
	public:
	E_VAR* variable, *lock;
	Expression* value;
	DefaultDeclaration(E_VAR* v, E_VAR* loc=NULL, Expression* e=NULL){
		lock = loc;
		variable = v;
		value = e;
	}

	ostream& write(ostream& f){
		f << "DefaultDeclaration(";
		if(lock!=NULL) lock->write(f) << " ";
		variable->write(f);
		if(value!=NULL) {
			f << "=";
			value->write(f);
		}
		f << ")";
		return f;
	}
};
inline ostream& operator<<(ostream& s, DefaultDeclaration* d){
	d->write(s);
	return s;
}


#endif /* DEFAULTDECLARATION_HPP_ */
