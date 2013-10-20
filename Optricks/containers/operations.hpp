/*
 * operations.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef OPERATIONS_HPP_
#define OPERATIONS_HPP_

#include "settings.hpp"

#define OPERATIONS_C_
class ouop{
	public:
		virtual ~ouop(){};
		virtual DATA apply(DATA a, RData& mod, PositionID id) = 0;
		ClassProto* returnType;
};

class ouopNative: public ouop{
	public:
		DATA (*temp)(DATA,RData&,PositionID);
		ouopNative(DATA (*fun)(DATA,RData&,PositionID),ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(DATA a,RData& m, PositionID id) override final{
			return temp(a,m,id);
		}
};

class ouopUser: public ouop{
	public:
		Function* func;
		ouopUser(Function* fun,ClassProto* a){
			func = fun;
			returnType = a;
		}
		DATA apply(DATA a,RData& m, PositionID id) override final{
			return DATA::getConstant(m.builder.CreateCall(func, a.getValue(m), "uop"), returnType);
		}
};

class obinop{
	public:
		virtual ~obinop(){};
		virtual DATA apply(DATA a, DATA b, RData& mod,PositionID) = 0;
		ClassProto* returnType;
};

class obinopNative : public obinop{
	public:
		DATA (*temp)(DATA,DATA,RData&,PositionID);
		obinopNative(DATA (*fun)(DATA,DATA,RData&,PositionID), ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(DATA a,DATA b, RData& m, PositionID id) override final{
			return temp(a,b,m,id);
		}
};

class obinopUser: public obinop{
	public:
		Function* func;
		obinopUser(Function* fun,ClassProto* a){
			func = fun;
			returnType = a;
		}
		DATA apply(DATA a,DATA b, RData& m, PositionID id) override final{
			return DATA::getConstant(m.builder.CreateCall2(func, a.getValue(m), b.getValue(m), "binop"), returnType);
		}
};
#endif /* OPERATIONS_HPP_ */
