/*
 * ofunction.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OFUNCTION_HPP_
#define OFUNCTION_HPP_

#include "oobject.hpp"

class ofunction:public oobject{
	public:
		ofunction() : oobject(functionClass){}
		virtual oobject* call(std::vector<oobject*>& a) = 0;
};
//TODO
class nativeFunction : public ofunction{

};
class lambdaFunction : public ofunction{

};
class userFunction : public ofunction{

};
#endif /* OFUNCTION_HPP_ */
