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
		virtual llvm::Value* getValue(RData& r, PositionID id)=0;
		virtual void setValue(llvm::Value* v, RData& r)=0;
		virtual llvm::Value* getPointer(RData& r,PositionID id) =0;
		virtual Location* getInner(RData& r, PositionID id, unsigned idx)=0;
		virtual Location* getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2)=0;
};

class StandardLocation : public Location{
	private:
	llvm::Value* position;
	public:
		~StandardLocation() override{};
		StandardLocation(llvm::Value* a):position(a){ assert(position); assert(position->getType()->isPointerTy());}
		llvm::Value* getValue(RData& r, PositionID id) override final;
		void setValue(llvm::Value* v, RData& r) override final;
		llvm::Value* getPointer(RData& r,PositionID id) override final{
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
	friend RData;
private:
	bool used;
	std::map<llvm::BasicBlock*,llvm::Value* > data;
	std::map<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> > phi;
	llvm::Value* position;
	llvm::Type* type;
	String varName;
public:
	~LazyLocation() override{};
	String getName(){ return varName; }
	LazyLocation(String nam, void* a, RData& r,llvm::Value* p, llvm::BasicBlock* b=NULL,llvm::Value* d=NULL,bool u = false):data(),position(p){
		used = u;
		varName = nam;
		assert(position);
		llvm::Type* t;
		assert(llvm::dyn_cast<llvm::PointerType>(p->getType()));
		type = ((llvm::PointerType*) p->getType())->getElementType();
		assert(type);
		if(b!=NULL){
			if(d!=NULL)
				assert(d->getType()==type);
			data[b] = d;
		}
		//if(d!=NULL) d->setName(name);
		r.flocs.find(r.builder.GetInsertBlock()->getParent())->second.push_back(this);
	}
	llvm::Value* getPointer(RData& r,PositionID id) override final{
		llvm::BasicBlock* me = r.builder.GetInsertBlock();
		auto found = data.find(me);
		if(found==data.end()){
			//not there -- create and insert phi-node unusable
			//BasicBlock* prev = r.pred.find(me->getParent())->second.find(me)->second;
			llvm::Value* v=NULL;
			//if(prev==NULL){
			llvm::PHINode* n = r.CreatePHI(type, 1U/*,name*/);
			phi.insert(std::pair<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >(me,std::pair<llvm::PHINode*,PositionID>(n,id)));
			v = n;
			/*} else {
				auto found2 = data.find(prev);
				if(found2==data.end())
					id.error("Attempting to get value from variable that has not been set");
				if(found2->second==NULL){
					r.builder.SetInsertPoint(prev);
					data[prev] = v = r.builder.CreateLoad(position);
					r.builder.SetInsertPoint(me);
				} else v = found->second;
			}*/
			llvm::Instruction* in;
			if(r.hadBreak()){
				in = & me->back();
			} else in = nullptr;
			auto STO = r.builder.CreateStore(v,position);
			if(in) STO->moveBefore(in);
			data[me] = NULL;
		}else if(found->second!=NULL){
			//there and usable -- load current into memory

			llvm::Instruction* in;
			if(r.hadBreak()){
				in = & me->back();
			} else in = nullptr;
			auto STO = r.builder.CreateStore(found->second,position);
			if(in) STO->moveBefore(in);
			data[me] = NULL;
			//TODO check if just can change iterator
		}
		used = true;
		return position;
	}
private:
		inline llvm::Value* getFastValue(RData& r, std::map<llvm::BasicBlock*,llvm::Value*>::iterator found,bool set=false){
			if(found->second==NULL){
				llvm::BasicBlock* bb;
				if(set){
					bb = r.builder.GetInsertBlock();
					r.builder.SetInsertPoint(found->first);
				} else bb = nullptr;
				assert(position);

				llvm::Instruction* in;
				if(r.hadBreak()){
					in = & found->first->back();
				} else in = nullptr;
				auto v = r.builder.CreateLoad(position);
				if(in) v->moveBefore(in);
				assert(v);
				assert(v->getType()==type);

				found->second = v;
				if(bb) r.builder.SetInsertPoint(bb);
				return v;
			} else{
				assert(found->second);
				if(found->second->getType()!=type){
					found->second->dump();
					found->second->getType()->dump();
					type->dump();
					cerr << endl << flush;
				}
				assert(found->second->getType()==type);
				return found->second;
			}
		}
public:
	llvm::Value* getValue(RData& r, PositionID id) override final{
		llvm::BasicBlock* me = r.builder.GetInsertBlock();
		auto found = data.find(me);
		if(found==data.end()){
			//not there -- create and insert phi-node unusable

			llvm::Value* v=NULL;

			llvm::PHINode* n = r.CreatePHI(type, 1U/*,name*/);
			phi.insert(std::pair<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >(me,std::pair<llvm::PHINode*,PositionID>(n,id)));
			v = n;

			assert(v);
			assert(v->getType()==type);
			data[me] = v;
			return v;
		}else return getFastValue(r,found,true);
	}
	void setValue(llvm::Value* v, RData& r) override final{
		assert(v);
		assert(v->getType()==type);
		//v->setName(name);
		llvm::BasicBlock* me = r.builder.GetInsertBlock();
		assert(!r.hadBreak());
		data[me] = v;
	}
	Location* getInner(RData& r, PositionID id, unsigned idx) override final{
		id.compilerError("Cannot use lazylocation on struct/pointer/array");
		exit(1);
	}
	Location* getInner(RData& r, PositionID id, unsigned idx1, unsigned idx2) override final{
		id.compilerError("Cannot use lazylocation on struct/pointer/array");
		exit(1);
	}
};

Location* getLazy(String name, RData& r,llvm::Value* p, llvm::BasicBlock* b=nullptr,llvm::Value* d=nullptr,bool u = false){
	assert(p->getType()->isPointerTy());
	llvm::Type* IT = ((llvm::PointerType*) p->getType())->getElementType();
	assert(!IT->isVoidTy());
	if(IT->isPointerTy() || IT->isIntegerTy() || IT->isHalfTy() || IT->isFloatTy()
			|| IT->isDoubleTy() || IT->isX86_FP80Ty() || IT->isFP128Ty() || IT->isPPC_FP128Ty()
			|| IT->isX86_MMXTy()){
		return new LazyLocation(name, nullptr,r,p,b,d,u);
	} else{//todo allow structs
		if(b && d){
			auto Parent = r.builder.GetInsertBlock();
			r.builder.SetInsertPoint(b);
			r.builder.CreateStore(d,p);
			if(Parent) r.builder.SetInsertPoint(Parent);
		}
		return new StandardLocation(p);
	}
}
#endif /* LOCATION_HPP_ */
