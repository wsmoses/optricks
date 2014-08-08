/*
 * ClosureScope.hpp
 *
 *  Created on: Aug 8, 2014
 *      Author: Billy
 */

#ifndef CLOSURESCOPE_HPP_
#define CLOSURESCOPE_HPP_
#include "Scopable.hpp"
class ClosureScope: public Scopable{
public:
	ClosureScope(Scopable* above,String n=""):Scopable(above,n){};

	const Data* getVariable(PositionID id, const String name) const;
	void setVariable(PositionID id, const String name, const Data* da);

	const Data* get(PositionID id, const String name, const T_ARGS&) const;

};



#endif /* CLOSURESCOPE_HPP_ */
