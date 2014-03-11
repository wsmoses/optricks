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

		Module lmod;
		IRBuilder<> builder;
		FunctionPassManager fpm;
		PassManager mpm;
		ExecutionEngine* exec;
		RData(): lmod("main",getGlobalContext()), builder(getGlobalContext())
		,fpm(&lmod),mpm(){
			lmod.setDataLayout("p:64:64:64");
			InitializeNativeTarget();
			exec = EngineBuilder(& lmod).create();
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
			BasicBlock* b = BasicBlock::Create(lmod.getContext(),Twine(name), F);
			return b;
		}
		inline Function* CreateFunctionD(String name,FunctionType* FT,Function::LinkageTypes L){
			Function* f = Function::Create(FT,L,Twine(name),&lmod);
			return f;
		}
		void FinalizeFunctionD(Function* f,bool debug=false){
			BasicBlock* Parent = builder.GetInsertBlock();
			if(Parent) builder.SetInsertPoint(Parent);
			if(debug){
				f->dump();
				cerr << endl << flush;
			}
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
			BasicBlock* b = BasicBlock::Create(lmod.getContext(), Twine(name), F);
			if(p!=nullptr){
				auto found = pred.find(F);
				assert(found!=pred.end() &&  "Compiler error -- could not find function in map");
				assert(this);
				assert(pred.size()>=0);
				assert(found->second.size()>=0);
				std::cout << found->second.size() << endl << flush;
				found->second.insert(std::pair<BasicBlock*,BasicBlock* >(b,p));
			}
			return b;
		}
		BasicBlock* getBlock(String name, JumpType jump, BasicBlock* bb, const Data* val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume=std::pair<BasicBlock*,BasicBlock*>(NULL,NULL));
};

RData rdata;
/*
Function* o_malloc;
Function* o_calloc;
Function* o_free;
Function* o_realloc;
Function* o_memset;
Function* o_memcpy;
//Function* o_glutInit;
Function* o_strlen;

template<typename T> inline ArrayRef<T> aref(std::initializer_list<T> li){
	return ArrayRef<T>(li.begin(), li.end());
}

#define types(...) ArrayRef<Type*>(aref<Type*>({__VA_ARGS__}))
#define PT(A) PointerType::getUnqual(A)
#define FT(A,B,C) FunctionType::get(A,B,C)
#define FC(A,B,C) C = Function::Create(A,EXTERN_FUNC,#B,r.lmod); r.exec->addGlobalMapping(C, (void*)(&B));
#define FCR(A,B) Function::Create(A,LOCAL_FUNC,B,r.lmod)
void initializeBaseFunctions(RData& r){

	FC(FT(C_POINTERTYPE, types(C_INTTYPE), false),malloc,o_malloc);

	/ ** %1 number of elements, each %2 long * /

	FC(FT(C_POINTERTYPE, types(SIZETYPE, SIZETYPE), false),calloc,o_calloc);

	FC(FT(VOIDTYPE, types(C_POINTERTYPE), false),free,o_free);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,SIZETYPE), false),realloc,o_realloc);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,C_INTTYPE,SIZETYPE), false),memset,o_memset);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,C_POINTERTYPE,SIZETYPE), false),memcpy,o_memcpy);

	FC(FT(C_INTTYPE,types(C_STRINGTYPE),false),strlen,o_strlen);
	/ *
	{
		Function* FC(FT(VOIDTYPE, types(PointerType::getUnqual(C_INTTYPE),PointerType::getUnqual(C_STRINGTYPE)), false), glutInit,o_glutInitI);
		o_glutInit = FCR(FT(VOIDTYPE, types(), false),"#glutInit");
		BasicBlock *Parent = r.builder.GetInsertBlock();
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", o_glutInit);
		r.builder.SetInsertPoint(BB);
		Value* v = r.builder.CreateAlloca(C_INTTYPE,getCInt(0));
		r.builder.CreateStore(getCInt(0),v);
		r.builder.CreateCall2(o_glutInitI, v, UndefValue::get(PointerType::getUnqual(C_STRINGTYPE)));
		r.builder.CreateRetVoid();
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
	}// * /
}
#undef FC
#undef FT
#undef PT
#undef types*/
#endif /* RDATA_HPP_ */
