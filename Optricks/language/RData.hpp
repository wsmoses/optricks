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
		BasicBlock* start;
		BasicBlock* end;
		AbstractClass* returnType;
		std::vector<std::pair<BasicBlock*,BasicBlock*>> resumes;
		std::vector<std::pair<BasicBlock*,const Data*> > endings;
		Jumpable(String n, TJump t, BasicBlock* s, BasicBlock* e, AbstractClass* p):
			name(n), toJump(t), start(s), end(e), returnType(p){

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
		/*Value *CreateConstGEP3_32(Value *Ptr, unsigned Idx0, unsigned Idx1,unsigned Idx2,
				const Twine &Name = "") {
			SmallVector<Value*,3> I(3);
			I[0]=ConstantInt::get(Type::getInt32Ty(getGlobalContext()), Idx0);
			I[1]=ConstantInt::get(Type::getInt32Ty(getGlobalContext()), Idx1);
			I[2]=ConstantInt::get(Type::getInt32Ty(getGlobalContext()), Idx2);
			builder.GetInsertBlock()->getParent()->dump();
			cerr << endl << flush;
			Ptr->dump();
			Ptr->getType()->dump();
			cerr << " <- mytype" << endl << flush;
			auto T = GetElementPtrInst::getIndexedType(Ptr->getType(),
					ArrayRef<uint64_t>(std::vector<uint64_t>({Idx0})));
			if(T) T->dump();
			else cerr << "null";
			cerr << endl << flush;
			T = GetElementPtrInst::getIndexedType(Ptr->getType(),
					ArrayRef<uint64_t>(std::vector<uint64_t>({Idx0,Idx1})));
			if(T) T->dump();
			else cerr << "null";
			cerr << endl << flush;
			T = GetElementPtrInst::getIndexedType(Ptr->getType(),
					ArrayRef<uint64_t>(std::vector<uint64_t>({Idx0,Idx1,Idx2})));
			if(T) T->dump();
			else cerr << "null";
			cerr << endl << flush;
			cerr << " " << Idx0 << "," << Idx1<< "," << Idx2<< " of " << endl << flush;

			return builder.CreateGEP(Ptr,I,Name);
		}*/
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
		//bool guarenteedReturn;
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
		void FinalizeFunction(Function* f,bool debug=false){
			BasicBlock* Parent = builder.GetInsertBlock();
			if(Parent) builder.SetInsertPoint(Parent);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}



			fpm.run(*f);
			flocs.erase(f);
			pred.erase(f);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}
		}
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
		BasicBlock* getBlock(String name, JumpType jump, BasicBlock* bb, const Data* val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume=std::pair<BasicBlock*,BasicBlock*>(NULL,NULL));
};

RData& getRData(){
	static RData rdata;
	return rdata;
}
#endif /* RDATA_HPP_ */
