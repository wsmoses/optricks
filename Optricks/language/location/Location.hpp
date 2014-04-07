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
	friend RData;
private:
	bool used;
	std::map<BasicBlock*,Value* > data;
	std::map<BasicBlock*,std::pair<PHINode*,PositionID> > phi;
	Value* position;
	Type* type;
public:
	~LazyLocation() override{};
	LazyLocation(void* a, RData& r,Value* p, BasicBlock* b=NULL,Value* d=NULL,bool u = false):data(),position(p){
		used = u;
		assert(position);
#ifndef NDEBUG
		Type* t;
		if(PointerType* pt = dyn_cast<PointerType>(p->getType())){
			t = pt->getElementType();
		} else assert(0 && "Cannot use non-pointer type for LazyLocation");
		type = t;
#else
		type = ((PointerType*) p->getType())->getElementType();
#endif
		assert(type);
#ifndef NDEBUG
		if(d!=NULL)
		assert(d->getType()==type);
#endif
		if(b!=NULL) data[b] = d;
		//if(d!=NULL) d->setName(name);
		r.flocs.find(r.builder.GetInsertBlock()->getParent())->second.push_back(this);
	}
	Value* getPointer(RData& r,PositionID id) override final{
		BasicBlock* me = r.builder.GetInsertBlock();
		auto found = data.find(me);
		if(found==data.end()){
			//not there -- create and insert phi-node unusable
			//BasicBlock* prev = r.pred.find(me->getParent())->second.find(me)->second;
			Value* v=NULL;
			//if(prev==NULL){
			PHINode* n = r.CreatePHI(type, 1U/*,name*/);
			phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >(me,std::pair<PHINode*,PositionID>(n,id)));
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
			r.builder.CreateStore(v,position);
			data[me] = NULL;
		}else if(found->second!=NULL){
			//there and usable -- load current into memory
			r.builder.CreateStore(found->second,position);
			data[me] = NULL;
			//TODO check if just can change iterator
		}
		used = true;
		return position;
	}
private:
		inline Value* getFastValue(RData& r, std::map<BasicBlock*,Value*>::iterator found,bool set=false){
			if(found->second==NULL){
				if(set) r.builder.SetInsertPoint(found->first);
				auto v = r.builder.CreateLoad(position);
				/*
				LoadInst* v = new LoadInst(position);
				if(Instruction* in = dyn_cast<Instruction>(position)){
					BasicBlock* b = in->getParent();
					if(b==found->first) v->insertAfter(in);
					else v->insertBefore(found->first->getFirstNonPHI());
				}
				else v->insertBefore(found->first->getFirstNonPHI());*/
				found->second = v;
				return v;
			} else return found->second;
		}
	public:
	Value* getValue(RData& r, PositionID id) override final{
		BasicBlock* me = r.builder.GetInsertBlock();
		auto found = data.find(me);
		if(found==data.end()){
			//not there -- create and insert phi-node unusable
			//BasicBlock* prev = r.pred.find(me->getParent())->second.find(me)->second;
			Value* v=NULL;
			//if(prev==NULL){
			PHINode* n = r.CreatePHI(type, 1U/*,name*/);
			phi.insert(std::pair<BasicBlock*,std::pair<PHINode*,PositionID> >(me,std::pair<PHINode*,PositionID>(n,id)));
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
			data[me] = v;
			return v;
		}else return getFastValue(r,found,true);
	}
	void setValue(Value* v, RData& r) override final{
		assert(v);
		assert(v->getType()==type);
		//v->setName(name);
		BasicBlock* me = r.builder.GetInsertBlock();
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

Location* getLazy(RData& r,Value* p, BasicBlock* b=NULL,Value* d=NULL,bool u = false){
	assert(p->getType()->isPointerTy());
	llvm::Type* IT = ((PointerType*) p->getType())->getElementType();
	assert(!IT->isVoidTy());
	if(IT->isPointerTy() || IT->isIntegerTy() || IT->isHalfTy() || IT->isFloatTy()
			|| IT->isDoubleTy() || IT->isX86_FP80Ty() || IT->isFP128Ty() || IT->isPPC_FP128Ty()
			|| IT->isX86_MMXTy()){
		return new LazyLocation(nullptr,r,p,b,d,u);
	} else{//todo allow structs
		if(b && d){
			auto* Parent = r.builder.GetInsertBlock();
			r.builder.SetInsertPoint(b);
			r.builder.CreateStore(d,p);
			if(Parent) r.builder.SetInsertPoint(Parent);
		}
		return new StandardLocation(p);
	}
}
#endif /* LOCATION_HPP_ */
