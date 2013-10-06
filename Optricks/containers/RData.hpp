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
	CONTINUE = 2
};

enum TJump{
	FUNC = 0,
	LOOP = 1
};

struct Jumpable {
	public:
		String name;
		TJump toJump;
		BasicBlock* start;
		BasicBlock* end;
		ClassProto* returnType;
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
		BasicBlock* getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, RData& rd);
};

class funcMap{
	private:
		std::vector<std::pair<Value*,FunctionProto*>> data;
	public:
		unsigned int size() const{
			return data.size();
		}
		funcMap(std::pair<Value*,FunctionProto*> a):data(){
			data.push_back(a);
		}
		funcMap():data(){
		}
		void add(FunctionProto* f, Value* t, PositionID id);
		bool set(FunctionProto* in, Value* t);
		std::pair<Value*,FunctionProto*>  get(FunctionProto* func,PositionID id) const;
};

#include "ClassProto.hpp"
BasicBlock* RData::getBlock(String name, JumpType jump, ClassProto* ret, BasicBlock* bb, Value* val, RData& rd){
	if(name==""){
		if(jump==RETURN){
			for(unsigned int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == FUNC){
					if(ret==NULL || !ret->hasCast(jumps[i]->returnType)){
						cerr << "Invalid return type, trying to use a " << ((ret==NULL)?"null":(ret->name)) << " instead of a " << jumps[i]->returnType->name << endl << flush;
						exit(0);
					}
					jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,ret->castTo(rd, val, jumps[i]->returnType)));
					return jumps[i]->end;
				}
				if(i == 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(unsigned int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i == 0){
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
