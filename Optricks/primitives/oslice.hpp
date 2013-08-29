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
		oslice(PositionID id, Statement* a, Statement* b, Statement* c): oobject(id, sliceClass),
				start(a), stop(b),step(c){
			if(step==0){
				cerr << "Step in slice cannot be 0";
				exit(0);
			}
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
		Value* evaluate(RData& a) override final{
			error("Not implemented : slice evaluate");
			return NULL;
		}
};

class E_SLICE : public Statement{
public:
	Statement* start, *stop, *step;
	E_SLICE(PositionID id, Statement* a, Statement* b, Statement* c): Statement(id, sliceClass),
			start(a), stop(b),step(c){}
	const Token getToken() const override{
		return T_SLICE;
	};
	Value* evaluate(RData& r) override final{
		error("Evaluate E_SLICE");
		/*
		oobject *a = start->evaluate(),
				*b = stop->evaluate(),
				*c = step->evaluate();
		if( !(a->returnType==intClass ||
				a->returnType==nullClass) ||
				!(b->returnType==intClass ||
				b->returnType==nullClass) ||
				!(c->returnType==intClass ||
				c->returnType==nullClass)){
			cerr << "Cannot make slice out of unknown type " << a->returnType << ":" << b->returnType << ":" << c->returnType;
			exit(0);
		}
		return new oslice(a,b,c);*/
		return NULL;
	}
	Statement* simplify() override{

		Statement *aa = start->simplify(),
				*bb = stop->simplify(),
				*cc = step->simplify();
		if(aa->getToken()!=T_OOBJECT || bb->getToken()!=T_OOBJECT || cc->getToken()!=T_OOBJECT)
			return new E_SLICE(filePos, aa,bb,cc);
		oobject *a = (oobject*)aa,
				*b = (oobject*)bb,
				*c = (oobject*)cc;
		if(!(a->returnType==intClass
				//|| a->returnType==nullClass
				)
				||
				!(b->returnType==intClass
				//		|| b->returnType==nullClass
				) ||
				!(c->returnType==intClass
				//		|| c->returnType==nullClass
				)
				){
			return new E_SLICE(filePos, aa,bb,cc);
		}
		return new oslice(filePos, a,b,c);
	}
	void registerClasses(RData& r) override final{
		start->registerClasses(r);
		stop->registerClasses(r);
		step->registerClasses(r);
	}
	void registerFunctionArgs(RData& r) override final{
		start->registerFunctionArgs(r);
		stop->registerFunctionArgs(r);
		step->registerFunctionArgs(r);
	}
	void registerFunctionDefaultArgs() override final{
		start->registerFunctionDefaultArgs();
		stop->registerFunctionDefaultArgs();
		step->registerFunctionDefaultArgs();
	}
	void resolvePointers() override final{
		start->resolvePointers();
		stop->resolvePointers();
		step->resolvePointers();
	}
	ClassProto* checkTypes(){
		if(start!=NULL){
			start->checkTypes();
			if(start->returnType!=intClass) error("Cannot have non-int type in slice "+start->returnType->name);
		}
		if(stop!=NULL){
			stop->checkTypes();
			if(stop->returnType!=intClass) error("Cannot have non-int type in slice "+stop->returnType->name);
		}
		if(step!=NULL){
			step->checkTypes();
			if(step->returnType!=intClass) error("Cannot have non-int type in slice "+step->returnType->name);
		}
		return returnType;
	}
	void write(ostream& f,String a="") const override{
		f << "E_SLICE(";
		start->write(f,"");
		f<<":";
		stop->write(f,"");
		f<<":";
		step->write(f,"");
		f << ")";
	}
	FunctionProto* getFunctionProto() override final{ return NULL; }
	void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
	ClassProto* getClassProto() override final{ return NULL; }
	void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
	AllocaInst* getAlloc() override final{ return NULL; };
	void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
	String getObjName() override final { error("Cannot get name"); return ""; }
	void setResolve(Value* v) override final { error("Cannot set resolve"); }
	Value* getResolve() override final { error("Cannot get resolve"); return NULL;}
};

#endif /* OSLICE_HPP_ */
