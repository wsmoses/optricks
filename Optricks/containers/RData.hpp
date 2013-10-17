/*
 * RData.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef RDATA_HPP_
#define RDATA_HPP_

#include "settings.hpp"

enum JumpType{
	RETURN = 0,
	BREAK = 1,
	CONTINUE = 2,
	YIELD = 3
};

enum TJump{
	FUNC = 0,
	LOOP = 1,
	GENERATOR = 2
};

struct Jumpable {
	public:
		String name;
		TJump toJump;
		BasicBlock* start;
		BasicBlock* end;
		ClassProto* returnType;
		std::vector<std::pair<BasicBlock*,BasicBlock*>> resumes;
		std::vector<std::pair<BasicBlock*,Value*> > endings;
		Jumpable(String n, TJump t, BasicBlock* s, BasicBlock* e, ClassProto* p):
			name(n), toJump(t), start(s), end(e), returnType(p){
		}
};

#define RDATA_C_
struct RData{
	private:
		std::vector<Jumpable*> jumps;
	public:
		RData();
		bool guarenteedReturn;
		Module* lmod;
		FunctionPassManager* fpm;
		PassManager* mpm;
		IRBuilder<> builder;
		ExecutionEngine* exec;
		void addJump(Jumpable* j){
			jumps.push_back(j);
		}
		Jumpable* popJump(){
			auto a = jumps.back();
			jumps.pop_back();
			return a;
		}
		BasicBlock* getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume=std::pair<BasicBlock*,BasicBlock*>(NULL,NULL));
};

class DATA{
	private:
		DataType type;
		PrivateData data;
		DATA(DataType t, void* v):type(t){data.pointer = v;};
	public:
		static DATA getFunction(Function* f){
			return DATA(R_FUNC, f);
		};
		static DATA getConstant(Value* v){
			return DATA(R_CONST, v);
		};
		static DATA getClass(ClassProto* c){
			return DATA(R_CLASS, c);
		};
		static DATA getGenerator(Statement* s){
			return DATA(R_GEN, s);
		};
		static DATA getLocation(Value* v){
			return DATA(R_LOC, v);
		};
		DataType getType() const{
			return type;
		};
		Value* getLocation() const{
			if(type!=R_LOC){
				cerr << "Cannot getLocation of non-location "<< type << endl << flush;
				exit(1);
			}
			if(data.pointer==NULL){
				cerr << "Cannot get NULL location" << endl << flush;
				exit(1);
			}
			return data.location;
		}
		Function* getMyFunction() const{
			if(type!=R_FUNC){
				cerr << "Cannot getFunction of non-function " << type << endl << flush;
				exit(1);
			}
			if(data.pointer==NULL){
				cerr << "Cannot get NULL function" << endl << flush;
				exit(1);
			}
			return data.function;
		}
		ClassProto* getMyClass() const{
			if(type!=R_CLASS){
				cerr << "Cannot getClass of non-class " << type << endl << flush;
				exit(1);
			}
			if(data.pointer==NULL){
				cerr << "Cannot get NULL class" << endl << flush;
				exit(1);
			}
			return data.classP;
		}
		Value* getValue(RData& r) const{
			assert(data.pointer!=NULL && "Cannot get NULL DATA");
			if(type==R_CONST) return data.constant;
			else if(type==R_FUNC) return data.function;
			else if(type==R_LOC) return r.builder.CreateLoad(data.location);
			else{
				cerr << "Cannot get Value of DataType " << type << endl << flush;
				exit(1);
			}
		};
		void setValue(RData& r, Value* v){
			if(type==R_CONST) data.constant = v;
			assert(data.pointer!=NULL && "Cannot set NULL DATA");
			if(type==R_LOC) r.builder.CreateStore(v, data.location);
			else{
				cerr << "Cannot set Value of DataType " << type << endl << flush;
				exit(1);
			}
		};
};

class funcMap{
	private:
		std::vector<std::pair<DATA,FunctionProto*>> data;
	public:
		unsigned int size() const{
			return data.size();
		}
		funcMap(std::pair<DATA,FunctionProto*> a):data(){
			data.push_back(a);
		}
		funcMap():data(){
		}
		void add(FunctionProto* f, DATA t, PositionID id);
		bool set(FunctionProto* in, DATA t);
		std::pair<DATA,FunctionProto*>  get(FunctionProto* func,PositionID id) const;
};

#include "ClassProto.hpp"
BasicBlock* RData::getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume){
	if(name==""){
		if(jump==RETURN || jump==YIELD){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == FUNC || jumps[i]->toJump==GENERATOR){
					if(ret==NULL || !ret->hasCast(jumps[i]->returnType)){
						id.error("Invalid return type, trying to use a " + ((ret==NULL)?"null":(ret->name)) + " instead of a " + jumps[i]->returnType->name);
						return NULL;
					}
					jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,ret->castTo(*this, val, jumps[i]->returnType)));
					jumps[i]->resumes.push_back(resume);
					return jumps[i]->end;
				}
				if(i <= 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i <= 0){
					cerr << "Error could not find continue/break block" << endl << flush;
					return NULL;
				}
			}
		}
	} else {
		cerr << "Error not done yet2" << endl << flush;
		exit(1);
		return NULL;
	}
}

#endif /* RDATA_HPP_ */
