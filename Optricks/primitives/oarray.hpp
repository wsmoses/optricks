/*
 * oarray.hpp
 *
 *  oobject*reated on: Jul 26, 2013
 *      Author: wmoses
 */

#ifndef OARRAY_HPP_
#define OARRAY_HPP_

#include "oobjectproto.hpp"

class oarray : public oobject{
	public:
		std::vector<oobject*> data;
		oarray(PositionID id, const std::vector<oobject*>& dat) : oobject(id,
				NULL //TODO arrayClass
		),data(dat){}
		ClassProto* checkTypes() override{
			error("Implement forced array typing");
			return NULL;
		}
		ConstantArray* evaluate(RData& a) override final{
			//TODO
			error("Array not implemented");
			return NULL;
		}
		oobject* simplify() override final{
			return this;
		}
		/*oobject* operator [] (oobject* a) override{
			if(a->returnType==intClass){
				oint* s = (oint*)a;
				return data[s->value];
			}
			else if(a->returnType==sliceClass){
				oslice* s = (oslice*)a;
				oarray* n = new oarray();
				int start = (s->start->returnType==intClass)?(((oint*)s->start)->value):0;
				int stop = (s->stop->returnType==intClass)?(((oint*)s->stop)->value):data.size();
				int step = (s->step->returnType==intClass)?(((oint*)s->step)->value):1;
				while(start<0) start+=data.size();
				while(stop<0) start+=data.size();
				for(int i = start; (step>0)?(i<stop):(i>stop); i+=step)
					n->data.push_back(data[i]);
				return n;
			}
			else{
				cerr << "Array index must be int or slice not " << a << " of class " << a->returnType;
				exit(0);
			}
		}*/
		void write(ostream& ss, String b) const override{
			ss << "[";
			if(data.size()>0) {
				for(unsigned int i = 0; i<data.size(); ++i){
					ss << data[i];
					if(i<data.size()-1) ss << ", ";
				}
			}
			ss << "]";
		}
};

class E_ARR : public Statement{
	public:
		std::vector<Statement*> values;
		E_ARR(PositionID id) : Statement(id,
				NULL //TODO arrayClass
				),values() { };
		E_ARR(PositionID id, const std::vector<Statement*>& a) : Statement(id,
				NULL //TODO arrayClass
				),values(a) { };
		const  Token getToken() const override{
			return T_ARR;
		};
		void write(ostream& f,String a="") const override{
			f << "[";
			for(unsigned int i = 0; i<values.size(); ++i){
				f << values[i];
				if(i<values.size()-1) f << ", ";
			}
			f << "]";
		}
		DATA evaluate(RData& a) override {
			//TODO
			error("E_ARR not implemented");
			/*
			oarray* n = new oarray();
			for(Statement* a: values){
				n->data.push_back(a->evaluate(a));
			}
			return n;*/
			return NULL;
		}

		Statement* simplify() override{

			std::vector<oobject*> vals;
			for(Statement* a: values){
				Statement* t = a->simplify();
				if(t->getToken()==T_OOBJECT)
					vals.push_back((oobject*)a);
				else{
					std::vector<Statement*> val2;
					for(Statement* b: values)
						val2.push_back(b->simplify());
					return new E_ARR(filePos, val2);
				}
			}
			return new oarray(filePos, vals);
		}

		void registerClasses(RData& r) override final{
			for(auto& a:values){
				a->registerClasses(r);
			}
		}
		void registerFunctionArgs(RData& r) override final{
			for(auto& a:values){
				a->registerFunctionArgs(r);
			}
		};
		void registerFunctionDefaultArgs() override final{
			for(auto& a:values){
				a->registerFunctionDefaultArgs();
			}
		};
		void resolvePointers() override final{
			for(auto& a: values){
				a->resolvePointers();
			}
		};
		ClassProto* checkTypes() override{
			for(auto& a:values){
				a->checkTypes();
			}
			error("Implement forced E_ARR typing");
//			return arrayClass;
		}
		FunctionProto* getFunctionProto() override final{ return NULL; }
		void setFunctionProto(FunctionProto* f) override final { error("Cannot set function prototype"); }
		ClassProto* getClassProto() override final{ return NULL; }
		void setClassProto(ClassProto* f) override final { error("Cannot set class prototype"); }
		AllocaInst* getAlloc() override final{ return NULL; };
		void setAlloc(AllocaInst* f) override final { error("Cannot set allocated instance"); }
		String getObjName() override final { error("Cannot get name"); return ""; }
		void setResolve(DATA v) override final { error("Cannot set resolve"); }
		DATA getResolve() override final { error("Cannot get resolve"); return NULL;}
};

#endif /* OARRAY_HPP_ */
