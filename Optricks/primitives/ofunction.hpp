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
		oclass* functionReturnType;
		std::vector<DefaultDeclaration> dec;
		ofunction(oclass* retType) : oobject(functionClass){
			functionReturnType = retType;
		}
		oobject* call(std::vector<oobject*>& vec){
			std::vector<oobject>
		}
		virtual oobject* _call(std::vector<oobject*>& a,std::vector<DefaultDeclaration>& de) = 0;
};
//TODO
class nativeFunction : public ofunction{

};
class lambdaFunction : public ofunction{

};
class userFunction : public ofunction{

};
#endif /* OFUNCTION_HPP_ */
