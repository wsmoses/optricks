/*
 * RData.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef RDATA_HPP_
#define RDATA_HPP_
#include "includes.hpp"

enum JumpType{
	RETURN,
	BREAK,
	CONTINUE,
	YIELD
};

enum TJump{
	LOOP,
	GENERATOR,
};

struct Jumpable {
	public:
		String name;
		TJump toJump;
		Scopable* scope;
		BasicBlock* start;
		BasicBlock* end;
		AbstractClass* returnType;
		std::vector<std::pair<BasicBlock*,BasicBlock*>> resumes;
		std::vector<std::pair<BasicBlock*,const Data*> > endings;
		Jumpable(String n, TJump t, Scopable* om, BasicBlock* s, BasicBlock* e, AbstractClass* p):
			name(n), toJump(t), scope(om), start(s), end(e), returnType(p){

		}
};


#define RDATA_C_
struct RData{
		friend LazyLocation;
	private:
		std::vector<Jumpable*> jumps;
		std::map<Function*,std::vector<LazyLocation*> > flocs;
		std::map<Function*,std::map<BasicBlock*,BasicBlock*> > pred;
	public:
		bool enableAsserts;
		Module* lmod;
		IRBuilder<> builder;
		FunctionPassManager fpm;
		PassManager mpm;
		ExecutionEngine* exec;
		const AbstractClass* functionReturn;
		RData(): enableAsserts(false),lmod(new Module("main",getGlobalContext())),
				builder(getGlobalContext())
		,fpm(lmod),mpm(),functionReturn(nullptr){
			lmod->setDataLayout("p:64:64:64");
			exec=nullptr;
			// Set up optimizers
			PassManagerBuilder pmb;
			pmb.Inliner = createFunctionInliningPass();
			pmb.OptLevel = 3;
			pmb.populateFunctionPassManager(fpm);
			pmb.populateModulePassManager(mpm);
		};
		PHINode* CreatePHI(Type *Ty, unsigned NumReservedValues, const Twine &Name = ""){
			PHINode* p = builder.CreatePHI(Ty,NumReservedValues,Name);
			assert(p);
			assert(p->getType()==Ty);
			Instruction* s = &(builder.GetInsertBlock()->front());
			if(s!=p) p->moveBefore(s);
			return p;
		}
		inline BasicBlock* CreateBlockD(String name,Function* F){
			BasicBlock* b = BasicBlock::Create(lmod->getContext(),Twine(name), F);
			return b;
		}
		inline Function* CreateFunctionD(String name,FunctionType* FT,Function::LinkageTypes L){
			Function* f = Function::Create(FT,L,Twine(name),lmod);
			return f;
		}
		inline Function* getExtern(String name, const AbstractClass* R, const std::vector<const AbstractClass*>& A, bool varArgs = false);
		inline Function* getExtern(String name, FunctionType* FT){
			return (Function*) lmod->getOrInsertFunction(StringRef(name), FT);
		}
		inline Value* getConstantCString(String name){
			static std::map<String,Value*> M;
			auto find = M.find(name);
			if(find!=M.end()) return find->second;
			else{
				Value* const V = builder.CreateGlobalStringPtr(StringRef(name));
				M.insert(std::pair<String,Value*>(name,V));
				return V;
			}
		}
		void FinalizeFunctionD(Function* f,bool debug=false){
			BasicBlock* Parent = builder.GetInsertBlock();
			if(Parent) builder.SetInsertPoint(Parent);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}
			assert(f);
			fpm.run(*f);
		}
		inline bool hadBreak(){
			BasicBlock* b = builder.GetInsertBlock();
			if(b->getInstList().size()==0) return false;
			return b->getInstList().back().isTerminator();
		}
		void recursiveFinalize(LazyLocation* ll, std::map<BasicBlock*,std::pair<PHINode*,PositionID> >::iterator toFix);
		void addJump(Jumpable* j){
			jumps.push_back(j);
		}
		Jumpable* popJump(){
			auto a = jumps.back();
			jumps.pop_back();
			return a;
		}

		Function* CreateFunction(String name,FunctionType* FT,Function::LinkageTypes L){
			Function* f = CreateFunctionD(name,FT,L);
			flocs.insert(std::pair<Function*,std::vector<LazyLocation*> >(f,std::vector<LazyLocation*>()));
			pred.insert(std::pair<Function*,std::map<BasicBlock*,BasicBlock*> >(f,std::map<BasicBlock*,BasicBlock*>()));
			return f;
		}
		void FinalizeFunction(Function* f,bool debug=false);
		void DeleteBlock(BasicBlock* b){

			b->removeFromParent();
			/*
			 * Function* F = builder.GetInsertBlock()->getParent();auto found = pred.find(F);
			assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
			found->second.std::pair<BasicBlock*,BasicBlock* >(b,p));
			}*/
		}
		BasicBlock* CreateBlock(String name, BasicBlock* p=NULL){
			Function* F = builder.GetInsertBlock()->getParent();
			BasicBlock* b = BasicBlock::Create(lmod->getContext(), Twine(name), F);
			if(p!=nullptr){
				auto found = pred.find(F);
				assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
				assert(this);
				assert(pred.size()>=0);
				assert(found->second.size()>=0);
				found->second.insert(std::pair<BasicBlock*,BasicBlock* >(b,p));
			}
			return b;
		}
		void makeJump(String name, JumpType jump, const Data* val, PositionID id);
};

RData& getRData(){
	static RData rdata;
	return rdata;
}
#endif /* RDATA_HPP_ */
