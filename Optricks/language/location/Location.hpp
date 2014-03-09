/*
 * Location.hpp
 *
 *  Created on: Feb 13, 2014
 *      Author: Billy
 */

#ifndef LOCATION_HPP_
#define LOCATION_HPP_
#include "../includes.hpp"

class Location{
	public:
		virtual ~Location(){};
		virtual Value* getValue(RData& r, PositionID id)=0;
		virtual void setValue(Value* v, RData& r)=0;
		virtual Value* getPointer(RData& r,PositionID id) =0;
};

class StandardLocation : public Location{
	private:
		Value* position;
	public:
		~StandardLocation() override{};
		StandardLocation(Value* a):position(a){}
		Value* getValue(RData& r, PositionID id) override final;
		void setValue(Value* v, RData& r) override final;
		Value* getPointer(RData& r,PositionID id) override final{
			return position;
		}
};

//todo have actual lazy
class LazyLocation: public Location{
private:
	Value* position;
public:
	~LazyLocation() override{};
	LazyLocation(RData& r,Value* p, BasicBlock* b=NULL,Value* d=NULL,bool u = false);
	Value* getValue(RData& r, PositionID id) override final;
	void setValue(Value* v, RData& r) override final;
	Value* getPointer(RData& r,PositionID id) override final{
		return position;
	}
};

#endif /* LOCATION_HPP_ */
