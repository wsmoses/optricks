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
		virtual Location* getInner(RData& r, PositionID id, unsigned idx)=0;
		virtual Location* getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2)=0;
};

class StandardLocation : public Location{
	private:
		Value* position;
	public:
		~StandardLocation() override{};
		StandardLocation(Value* a):position(a){ assert(position); assert(position->getType()->isPointerTy());}
		Value* getValue(RData& r, PositionID id) override final;
		void setValue(Value* v, RData& r) override final;
		Value* getPointer(RData& r,PositionID id) override final{
			return position;
		}
		Location* getInner(RData& r, PositionID id, unsigned idx) override final{
			return new StandardLocation(r.builder.CreateConstGEP1_32(position, idx));
		}
		Location* getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2) override final{
			return new StandardLocation(r.builder.CreateConstGEP2_32(position, idx1, idx2));
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
	Location* getInner(RData& r, PositionID id, unsigned idx) override final{
		return new StandardLocation(r.builder.CreateConstGEP1_32(position, idx));
	}
	Location* getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2) override final{
		return new StandardLocation(r.builder.CreateConstGEP2_32(position, idx1, idx2));
	}
};

#endif /* LOCATION_HPP_ */
