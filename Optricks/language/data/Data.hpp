/*
 * DATA.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef DATA_HPP_
#define DATA_HPP_
#include "../includes.hpp"
#include "../evaluatable/Evaluatable.hpp"
class Location{
	public:
		virtual ~Location()=0;
		virtual Value* getValue(RData& r, PositionID id)=0;
		virtual void setValue(Value* v, RData& r)=0;
		virtual Value* getPointer(RData& r,PositionID id) =0;
};

#define DATA_C_
class Data: public Evaluatable{
public:
	const DataType type;
		Data(const DataType t):type(t){}
	public:
		virtual ~Data(){};
		inline const Data* evaluate(RData& r) const override final{
			return this;
		}
		virtual const Data* toValue(RData& r, PositionID id) const=0;
		virtual const Data* castTo(RData& r, const AbstractClass* const right, PositionID id) const=0;
		virtual const AbstractClass* getReturnType() const=0;
		virtual Value* getValue(RData& r, PositionID id) const=0;
		virtual Value* castToV(RData& r, const AbstractClass* const right, PositionID id) const=0;
		llvm::Value* evalV(RData& r,PositionID id) const override final{
			return getValue(r,id);
		}
		virtual const Data* callFunction(RData& r, PositionID id, const std::vector<Evaluatable*>& args) const =0;
		/**
		 * Returns the class that this represents, if it represents a class
		 */
		virtual const AbstractClass* getMyClass(RData& r, PositionID id)const =0;
		//virtual AbstractFunction* getMyFunction(PositionID id) const=0;
		/*FunctionProto* getFunctionType() const{
			if(type!=R_FUNC && type!=R_GEN) PositionID(0,0,"<start.gft>").error("Could not gtf "+str<DataType>(type));
			assert(info.funcType !=NULL);
			return info.funcType;
		}
		E_GEN* getMyGenerator() const{
			if(type!=R_GEN){
				cerr << "Cannot getGenerator of non-gen " << type << endl << flush;
				exit(1);
			}
			assert(data.pointer);
			return data.generator;
		}
		Function* getMyFunction() const{
			if(type!=R_FUNC){
				cerr << "Cannot getFunction of non-function " << type << endl << flush;
				exit(1);
			}
			assert(data.pointer!=NULL);
			return data.function;
		}
		AbstractClass* getMyClass() const;
		*/
};

#endif /* DATA_HPP_ */
