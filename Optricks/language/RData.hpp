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
	FUNC,
	GENERATOR,
};

template<> String str<TJump>(TJump d){
	String ret;
	switch(d){
		case LOOP: ret = "LOOP";
			break;
		case FUNC: ret = "FUNC";
			break;
		case GENERATOR: ret = "GENERATOR";
			break;
	}
	return ret;
}

struct Jumpable {
	public:
		String name;
		TJump toJump;
		Scopable* scope;
		llvm::BasicBlock* start;
		llvm::BasicBlock* end;
		const AbstractClass* returnType;
		std::vector<std::pair<llvm::BasicBlock*,llvm::BasicBlock*>> resumes;
		std::vector<std::pair<llvm::BasicBlock*,const Data*> > endings;
		Jumpable(String n, TJump t, Scopable* om, llvm::BasicBlock* s, llvm::BasicBlock* e, const AbstractClass* p):
			name(n), toJump(t), scope(om), start(s), end(e), returnType(p){

		}
};


#define RDATA_C_
struct RData{
		friend LazyLocation;
	private:
		std::vector<Jumpable*> jumps;
		std::map<llvm::Function*,std::vector<LazyLocation*> > flocs;
		std::map<llvm::Function*,std::map<llvm::BasicBlock*,llvm::BasicBlock*> > pred;
		llvm::ExecutionEngine* exec;
	public:
		bool enableAsserts;
		llvm::Module* lmod;
		llvm::IRBuilder<> builder;
		llvm::FunctionPassManager fpm;
		llvm::PassManager mpm;
		bool debug;
		RData(): enableAsserts(false),lmod(new llvm::Module("main",llvm::getGlobalContext())),
				builder(llvm::getGlobalContext())
		,fpm(lmod),mpm(){
			lmod->setDataLayout("p:64:64:64");
			exec=nullptr;
			// Set up optimizers
			llvm::PassManagerBuilder pmb;
			pmb.Inliner = llvm::createFunctionInliningPass();
			pmb.OptLevel = 3;
			pmb.populateFunctionPassManager(fpm);
			pmb.populateModulePassManager(mpm);
			debug = false;

		};
		llvm::ExecutionEngine* getExec(){
			if(exec) return exec;
			else{

				llvm::InitializeNativeTarget();
				//llvm::InitializeAllTargets();
				String erS;
				exec = llvm::EngineBuilder(lmod).setErrorStr(& erS).create();
				if(!exec){
					cerr << erS << endl << flush;
					exit(1);
				}
				for(const auto& a: toPut){
					assert(exec);
					assert(a.first);
					assert(a.second);
					exec->updateGlobalMapping(a.first,a.second);
				}
				return exec;
			}
		}
		llvm::PHINode* CreatePHI(llvm::Type *Ty, unsigned NumReservedValues, const llvm::Twine &Name = ""){
			llvm::PHINode* p = builder.CreatePHI(Ty,NumReservedValues,Name);
			assert(p);
			assert(p->getType()==Ty);
			llvm::Instruction* s = &(builder.GetInsertBlock()->front());
			if(s!=p) p->moveBefore(s);
			return p;
		}
		inline llvm::BasicBlock* CreateBlockD(String name,llvm::Function* F){
			llvm::BasicBlock* b = llvm::BasicBlock::Create(lmod->getContext(),llvm::Twine(name), F);
			return b;
		}
		inline llvm::Function* CreateFunctionD(String name,llvm::FunctionType* FT,llvm::Function::LinkageTypes L){
			llvm::Function* f = llvm::Function::Create(FT,L,llvm::Twine(name),lmod);
			return f;
		}
		void error(String s);
		inline llvm::Constant* getExtern(String name, const AbstractClass* R, const std::vector<const AbstractClass*>& A, bool varArgs = false, String lib="");
		std::map<llvm::Function*, void*> toPut;
		inline llvm::Constant* getExtern(String name, llvm::FunctionType* FT, String lib=""){
			//TODO actually check library
			assert(FT);
			for(unsigned i=0; i<FT->getNumParams(); i++)
				assert(FT->getParamType(i));
			auto G = lmod->getOrInsertFunction(llvm::StringRef(name), FT);
			assert(G);
			//getExec();
if(llvm::Function* F = llvm::dyn_cast<llvm::Function>(G)){
			if(false){}
#define MAP(X) else if(name==#X){ assert((void*)(&X)); if(!exec) toPut[F] = (void*)(&X); else exec->updateGlobalMapping(F,(void*)(&X)); }
			MAP(opendir)
			MAP(readdir)
			MAP(closedir)
			MAP(stat)
#ifdef USE_OPENGL
//#pragma message "Using OpenGL"
			MAP(glutInit)
#else
//#pragma message "Not Using OpenGL"
#endif
#ifdef USE_SDL
//#pragma message "Using SDL"
#if defined(WIN32) || defined(_WIN32)
			MAP(PlaySound)
			MAP(mciSendString)
			MAP(CreateEvent)
			MAP(ResetEvent)
#endif
			MAP(SDL_Init)
			MAP(SDL_InitSubSystem)
			MAP(SDL_Quit)
			MAP(SDL_QuitSubSystem)
			MAP(SDL_GetError)
			MAP(SDL_ClearError)
			MAP(SDL_ClearError)
			MAP(Mix_OpenAudio)
			MAP(Mix_PlayChannelTimed)
			MAP(Mix_CloseAudio)
			MAP(Mix_Playing)
			MAP(Mix_LoadWAV_RW)
#else
//#pragma message "Not Using SDL"
#endif
#undef MAP
		}
			return G;
		}
		inline llvm::Value* getConstantCString(String name){
			static std::map<String,llvm::Value*> M;
			auto find = M.find(name);
			if(find!=M.end()) return find->second;
			else{
				llvm::Value* const V = builder.CreateGlobalStringPtr(llvm::StringRef(name));
				M.insert(std::pair<String,llvm::Value*>(name,V));
				return V;
			}
		}
		void FinalizeFunctionD(llvm::Function* f){
			llvm::BasicBlock* Parent = builder.GetInsertBlock();
			if(Parent) builder.SetInsertPoint(Parent);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}
			assert(f);
			fpm.run(*f);
		}
		inline bool hadBreak(){
			llvm::BasicBlock* b = builder.GetInsertBlock();
			if(b->getInstList().size()==0) return false;
			return b->getInstList().back().isTerminator();
		}
		llvm::Value* phiRecur(std::set<llvm::PHINode*> done, std::vector<LazyLocation*>& V, unsigned idx, llvm::PHINode* target,bool prop);
		llvm::Value* getLastValueOf(std::set<llvm::PHINode*> done, std::vector<LazyLocation*>& V, unsigned idx, llvm::BasicBlock* b, PositionID id);
		void addJump(Jumpable* j){
			assert(j);
			if(/*j->toJump==FUNC || */j->toJump==GENERATOR)
				assert(j->returnType);
			jumps.push_back(j);
		}
		Jumpable* popJump(){
			assert(jumps.size()>0);
			auto a = jumps.back();
			assert(a);
			jumps.pop_back();
			return a;
		}

		llvm::Function* CreateFunction(String name,llvm::FunctionType* FT,llvm::Function::LinkageTypes L){
			llvm::Function* f = CreateFunctionD(name,FT,L);
			flocs.insert(std::pair<llvm::Function*,std::vector<LazyLocation*> >(f,std::vector<LazyLocation*>()));
			pred.insert(std::pair<llvm::Function*,std::map<llvm::BasicBlock*,llvm::BasicBlock*> >(f,std::map<llvm::BasicBlock*,llvm::BasicBlock*>()));
			return f;
		}
		void FinalizeFunction(llvm::Function* f);
		void DeleteBlock(llvm::BasicBlock* b){

			b->removeFromParent();
			/*
			 * Function* F = builder.GetInsertBlock()->getParent();auto found = pred.find(F);
			assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
			found->second.std::pair<BasicBlock*,BasicBlock* >(b,p));
			}*/
		}
		llvm::BasicBlock* CreateBlock(String name, llvm::BasicBlock* p=NULL){
			llvm::Function* F = builder.GetInsertBlock()->getParent();
			llvm::BasicBlock* b = llvm::BasicBlock::Create(lmod->getContext(), llvm::Twine(name), F);
			if(p!=nullptr){
				auto found = pred.find(F);
				assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
				assert(this);
				assert(pred.size()>=0);
				assert(found->second.size()>=0);
				found->second.insert(std::pair<llvm::BasicBlock*,llvm::BasicBlock* >(b,p));
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
