/*
 * operations.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef OPERATIONS_HPP_
#define OPERATIONS_HPP_

#include "settings.hpp"

#include "RData.hpp"

#define OPERATIONS_C_
class ouop{
	public:
		virtual ~ouop(){};
		virtual DATA apply(DATA a, RData& mod) = 0;
		ClassProto* returnType;
};

class ouopNative: public ouop{
	public:
		DATA (*temp)(DATA,RData&);
		ouopNative(DATA (*fun)(DATA,RData&),ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(DATA a,RData& m){
			return temp(a,m);
		}
};

class ouopUser: public ouop{
	public:
		Function* func;
		ouopUser(Function* fun,ClassProto* a){
			func = fun;
			returnType = a;
		}
		DATA apply(DATA a,RData& m){
			return m.builder.CreateCall(func, a, "uop");
		}
};

class obinop{
	public:
		virtual ~obinop(){};
		virtual DATA apply(DATA a, DATA b, RData& mod) = 0;
		ClassProto* returnType;
};

class obinopNative : public obinop{
	public:
		DATA (*temp)(DATA,DATA,RData&);
		obinopNative(DATA (*fun)(DATA,DATA,RData&), ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(DATA a,DATA b, RData& m){
			return temp(a,b,m);
		}
};

class obinopUser: public obinop{
	public:
		Function* func;
		obinopUser(Function* fun,ClassProto* a){
			func = fun;
			returnType = a;
		}
		DATA apply(DATA a,DATA b, RData& m) override final{
			return m.builder.CreateCall2(func, a, b, "binop");
		}
};
#endif /* OPERATIONS_HPP_ */
