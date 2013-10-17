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
		virtual DATA apply(Value* a, RData& mod) = 0;
		DATA apply(DATA a, RData& mod){
			return apply(a.getValue(mod), mod);
		}
		ClassProto* returnType;
};

class ouopNative: public ouop{
	public:
		DATA (*temp)(Value*,RData&);
		ouopNative(DATA (*fun)(Value*,RData&),ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(Value* a,RData& m) override final{
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
		DATA apply(Value* a,RData& m) override final{
			return DATA::getConstant(m.builder.CreateCall(func, a, "uop"));
		}
};

class obinop{
	public:
		virtual ~obinop(){};
		virtual DATA apply(Value* a, Value* b, RData& mod) = 0;
		DATA apply(DATA a, DATA b, RData& mod){
			return apply(a.getValue(mod), b.getValue(mod), mod);
		}
		ClassProto* returnType;
};

class obinopNative : public obinop{
	public:
		DATA (*temp)(Value*,Value*,RData&);
		obinopNative(DATA (*fun)(Value*,Value*,RData&), ClassProto* a){
			temp = fun;
			returnType = a;
		}
		DATA apply(Value* a,Value* b, RData& m) override final{
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
		DATA apply(Value* a,Value* b, RData& m) override final{
			return DATA::getConstant(m.builder.CreateCall2(func, a, b, "binop"));
		}
};
#endif /* OPERATIONS_HPP_ */
