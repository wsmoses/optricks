/*
 * oslice.hpp
 *
 *  Created on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OSLICE_HPP_
#define OSLICE_HPP_

#include "oobjectproto.hpp"

class oslice : public oobject{
	public:
		Statement* start, *stop, *step;
		virtual ~oslice(){};
		oslice(PositionID id, Statement* a, Statement* b, Statement* c): oobject(id, sliceClass),
				start(a), stop(b),step(c){
			//if(step==0){
			//	cerr << "Step in slice cannot be 0";
			//	exit(0);
			//}
		}
		oobject* simplify() override final{
			return this;
		}
		void write(ostream& ss, String b) const override{
			ss << "[";
			if(start!=NULL && start!=VOID) ss << start;
			ss << ":";
			if(stop!=NULL && step!=VOID) ss << stop;
			ss << ":";
			if(step!=NULL && step!=VOID) ss << step;
			ss << "]";
		}
		DATA evaluate(RData& a) override final{
			error("Not implemented : slice evaluate");
			return NULL;
		}
};

#endif /* OSLICE_HPP_ */
