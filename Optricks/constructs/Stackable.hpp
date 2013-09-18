/*
 * Stackable.hpp
 *
 *  Created on: Jul 28, 2013
 *      Author: wmoses
 */

#ifndef STACKABLE_HPP_
#define STACKABLE_HPP_
#include "../containers/all.hpp"
class Stackable{
	public:
		//virtual ~Stackable();
		virtual const Token getToken() const = 0;
		virtual void write(ostream& s,String start="") const = 0;
};
inline ostream& operator << (ostream&f, Stackable& s){
	s.write(f,"");
	return f;
}
inline ostream& operator << (ostream&f, Stackable* s){
	s->write(f,"");
	return f;
}

#endif /* STACKABLE_HPP_ */
