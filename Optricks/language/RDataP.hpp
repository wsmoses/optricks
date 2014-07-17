/*
 * RDataP.hpp
 *
 *  Created on: Mar 3, 2014
 *      Author: Billy
 */

#ifndef RDATAP_HPP_
#define RDATAP_HPP_

#include "RData.hpp"
#include "./class/AbstractClass.hpp"
#include "./class/builtin/IntClass.hpp"
#include "./class/builtin/VoidClass.hpp"
#include "./data/VoidData.hpp"
#include "../operators/Deconstructor.hpp"

//MAX IS INCLUDED
llvm::Value* RData::randInt(llvm::Value* MAX,llvm::Value* REAL_MT,llvm::Value* REAL_IDX_P){
	assert(MAX->getType()==INT32TYPE);
	llvm::Value* MASK;
	if(auto VAL = llvm::dyn_cast<llvm::ConstantInt>(MAX)){
		auto max = VAL->getLimitedValue();
		if(max==0) return getInt32(0);
		else if( ( max & (max + 1) ) == 0){
			//is one less than power of two (e.g. is all ones)
			return builder.CreateAnd(rand(REAL_MT, REAL_IDX_P), max);
		} else {
			//TODO there is a weird error here...
			auto bits = 32 - llvm::countLeadingZeros<uint32_t>((uint32_t)max);
			uint64_t mask = bits;
			mask = (1 << mask) - 1;
			MASK = llvm::ConstantInt::get(INT32TYPE,mask,false);
		}
	} else {
		llvm::SmallVector<llvm::Type*,1> ar(2);
		ar[0] = INT32TYPE;
		ar[1] = BOOLTYPE;
		llvm::Value* BITS = builder.CreateCall2(llvm::Intrinsic::getDeclaration(getRData().lmod, llvm::Intrinsic::ctlz,ar),MAX,llvm::ConstantInt::get(BOOLTYPE,0,false));
		BITS = builder.CreateSub(llvm::ConstantInt::get(INT32TYPE,32,false),BITS);
		MASK = builder.CreateSub(builder.CreateShl(getInt32(1),BITS),getInt32(1));
	}
	auto F = builder.GetInsertBlock()->getParent();
	llvm::BasicBlock* LOOP = CreateBlockD("LOOP", F);
	llvm::BasicBlock* END_LOOP = CreateBlockD("end_loop", F);

	builder.CreateBr(LOOP);
	builder.SetInsertPoint(LOOP);
	auto R = builder.CreateAnd(this->rand(REAL_MT, REAL_IDX_P),MASK);
	builder.CreateCondBr(builder.CreateICmpULE(R, MAX), END_LOOP, LOOP);

	builder.SetInsertPoint(END_LOOP);
	return R;
}

#define N 624
#define M 397
llvm::CallInst* RData::seed(llvm::Value* REAL_S, llvm::Value* REAL_MT,llvm::Value* REAL_IDX_P){
	static llvm::Function* F=nullptr;
	if(REAL_MT==nullptr){
		if(GLOBAL_MT==nullptr){
			GLOBAL_IDX_P = new llvm::GlobalVariable(*lmod,INT32TYPE,false,llvm::GlobalValue::PrivateLinkage,
					llvm::ConstantInt::get(INT32TYPE,N,false));

			GLOBAL_MT = new llvm::GlobalVariable(*lmod,llvm::ArrayType::get(INT32TYPE,N),false,llvm::GlobalValue::PrivateLinkage,llvm::UndefValue::get(llvm::ArrayType::get(INT32TYPE,N)));
		}
		REAL_MT = builder.CreatePointerCast(GLOBAL_MT, llvm::PointerType::getUnqual(INT32TYPE));
		REAL_IDX_P = GLOBAL_IDX_P;
	}
	if(F==nullptr){
		auto PARENT = builder.GetInsertBlock();
		llvm::SmallVector<llvm::Type*,3> rand_args(3);
		rand_args[0] = llvm::PointerType::getUnqual(INT32TYPE);
		rand_args[1] = llvm::PointerType::getUnqual(INT32TYPE);
		rand_args[2] = INT32TYPE;
		F = CreateFunctionD("_seed32", llvm::FunctionType::get(VOIDTYPE, rand_args, false), LOCAL_FUNC);
		llvm::BasicBlock* ENTRY = CreateBlockD("entry", F);
		builder.SetInsertPoint(ENTRY);
		llvm::Value* MT, * IDX_P, *S;
		{
			llvm::Function::arg_iterator AI = F->arg_begin();
			AI->setName("mt");
			MT = AI;
			++AI;
			AI->setName("idx_p");
			IDX_P = AI;
			++AI;
			AI->setName("seed");
			S = AI;
		}

		builder.CreateStore(S, MT);
		llvm::BasicBlock* LOOP1 = CreateBlockD("LOOP", F);
		builder.CreateBr(LOOP1);
		builder.SetInsertPoint(LOOP1);

		llvm::BasicBlock* END_LOOP = CreateBlockD("end_loop", F);

		auto MT1 = builder.CreatePHI(INT32TYPE,2);
		MT1->addIncoming(getInt32(0),ENTRY);
		auto P1 = builder.CreateAdd(MT1,getInt32(1));
		MT1->addIncoming(P1, LOOP1);

		auto MTKK_P = builder.CreateGEP(MT,P1);

		auto PREV = builder.CreateLoad(builder.CreateGEP(MT, MT1));
		builder.CreateStore(
			builder.CreateAdd(
				builder.CreateMul(llvm::ConstantInt::get(INT32TYPE,1812433253UL,false),
					builder.CreateXor(PREV,builder.CreateLShr(PREV, (uint64_t)30) )
				),
				P1
			),
			MTKK_P
		);
		builder.CreateCondBr(builder.CreateICmpEQ(P1, getInt32(N-1)), END_LOOP, LOOP1);
		builder.SetInsertPoint(END_LOOP);
		builder.CreateStore(getInt32(N), IDX_P);
		builder.CreateRetVoid();
		this->FinalizeFunctionD(F);
		if(PARENT) builder.SetInsertPoint(PARENT);
	}
	return builder.CreateCall3(F, REAL_MT, REAL_IDX_P,REAL_S);
}

llvm::Value* RData::rand(llvm::Value* REAL_MT,llvm::Value* REAL_IDX_P){
	static llvm::Function* F=nullptr;
	auto PARENT = builder.GetInsertBlock();

	if(REAL_MT==nullptr) {
		static bool seeded = false;
		if(!seeded) {
			seeded = true;
		llvm::Function* TO_ADD = lmod->getFunction(":input_");
		if(TO_ADD==nullptr) TO_ADD = lmod->getFunction("main");
		assert(TO_ADD);
		assert(PARENT);
		auto& BB = TO_ADD->front();
		if(BB.empty())
			builder.SetInsertPoint(& BB);
		else
			builder.SetInsertPoint(& BB.front());
		/*
		llvm::SmallVector<llvm::Type*,0> args(0);

		F = llvm::Function::Create(llvm::FunctionType::get(C_INTTYPE, args, false), llvm::Function::ExternalLinkage,
				"rand", lmod);
		assert(F->getName()=="rand");



		llvm::SmallVector<llvm::Type*,1> s_args(1);
		s_args[0] = C_INTTYPE;

		auto SRAND = getExtern("srand", llvm::FunctionType::get(VOIDTYPE, s_args, false));
		*/
/*
		auto VV = r.builder.CreateCall(llvm::Intrinsic::getDeclaration(getRData().lmod,llvm::Intrinsic::
#if UINT_MAX == UINT16_MAX
				x86_rdseed_16
#elif UINT_MAX == UINT32_MAX
				x86_rdseed_32
#elif UINT_MAX == UINT64_MAX
				x86_rdseed_64
#endif
		,llvm::SmallVector<llvm::Type*,0>()));
		assert(VV);
		auto C = r.builder.CreateExtractValue(VV, llvm::SmallVector<unsigned int, 1>(1, (unsigned int)0));

		r.printf("rdseed %u of %u\n", C, r.builder.CreateExtractValue(VV, llvm::SmallVector<unsigned int, 1>(1, (unsigned int)1)));
		r.builder.GetInsertBlock()->dump();
		fflush(0);
*/


		auto TIME_T = llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(time_t));

		llvm::SmallVector<llvm::Type*,1> t_args(1);
		auto tmp = llvm::PointerType::getUnqual(TIME_T);
		t_args[0] = llvm::PointerType::getUnqual(TIME_T);

		llvm::Value* CLOCK = getExtern("time", llvm::FunctionType::get(TIME_T, t_args, false));
		llvm::Instruction* CA = builder.CreateCall(CLOCK, llvm::ConstantPointerNull::get(tmp));
		auto CAST = builder.CreateZExtOrTrunc(CA,INT32TYPE);
		if(auto INST = llvm::dyn_cast<llvm::Instruction>(CAST)){
			INST->removeFromParent();
			INST->insertAfter(CA);
			CA = INST;
		}
		auto CALL = this->seed(builder.CreateZExtOrTrunc(CA,INT32TYPE));
		CALL->removeFromParent();
		CALL->insertAfter(CA);
		}
		assert(GLOBAL_MT);
		assert(GLOBAL_IDX_P);
		assert(REAL_IDX_P==nullptr);
		REAL_MT = builder.CreatePointerCast(GLOBAL_MT, llvm::PointerType::getUnqual(INT32TYPE));
		REAL_IDX_P = GLOBAL_IDX_P;
		/*
		builder.CreateCall(SRAND, builder.CreateZExtOrTrunc(C, C_INTTYPE));
		builder.CreateCall(F);*/
	}
	if(F==nullptr){
		llvm::Value* MATRIX_A   = llvm::ConstantInt::get(INT32TYPE,0x9908b0dfUL,false);   /* constant vector a */
		llvm::Value* UPPER_MASK = llvm::ConstantInt::get(INT32TYPE,0x80000000UL,false); /* most significant w-r bits */
		llvm::Value* LOWER_MASK = llvm::ConstantInt::get(INT32TYPE,0x7fffffffUL,false); /* least significant r bits */

		llvm::SmallVector<llvm::Type*,2> rand_args(2);
		rand_args[0] = llvm::PointerType::getUnqual(INT32TYPE);
		rand_args[1] = llvm::PointerType::getUnqual(INT32TYPE);
		F = CreateFunctionD("_rand32", llvm::FunctionType::get(INT32TYPE, rand_args, false), LOCAL_FUNC);
		llvm::BasicBlock* BB = CreateBlockD("entry", F);
		builder.SetInsertPoint(BB);
		assert(REAL_MT);
		assert(REAL_MT->getType()->isPointerTy());
		assert(((llvm::PointerType*)REAL_MT->getType())->getPointerElementType()->isIntegerTy(32));

		llvm::BasicBlock* LOOP1= CreateBlockD("loop1", F);
		llvm::BasicBlock* GEN_NUM = CreateBlockD("gen_num", F);
		llvm::Value* MT, * IDX_P;
		{
			llvm::Function::arg_iterator AI = F->arg_begin();
			AI->setName("mt");
			MT = AI;
			++AI;
			AI->setName("idx_p");
			IDX_P = AI;
		}
		llvm::Value* IDX = builder.CreateLoad(IDX_P);
		builder.CreateCondBr(builder.CreateICmpUGE(IDX, llvm::ConstantInt::get(IDX->getType(), N)), LOOP1, GEN_NUM);
		builder.SetInsertPoint(LOOP1);

		auto KK = builder.CreatePHI(INT32TYPE,2);
		auto LONG_ZERO  = llvm::ConstantInt::get(INT32TYPE,0,false);
		auto LONG_ONE  = llvm::ConstantInt::get(INT32TYPE,1,false);
		KK->addIncoming(LONG_ZERO,BB);
		auto KKP1 = builder.CreateAdd(KK,LONG_ONE);
		auto MTKK_P = builder.CreateGEP(MT,KK);
		llvm::Value* Y = builder.CreateOr(
				builder.CreateAnd(builder.CreateLoad(MTKK_P),UPPER_MASK),
				builder.CreateAnd(builder.CreateLoad(builder.CreateGEP(MT,KKP1)),LOWER_MASK));
		builder.CreateStore(builder.CreateXor(builder.CreateXor(builder.CreateLoad(
				builder.CreateGEP(MT,
						builder.CreateAdd(KK,llvm::ConstantInt::get(INT32TYPE,M,false))
				)),builder.CreateLShr(Y,(uint64_t)1)),
				builder.CreateSelect(builder.CreateTrunc(Y, BOOLTYPE),MATRIX_A,LONG_ZERO)),MTKK_P);
		KK->addIncoming(KKP1, LOOP1);
		llvm::BasicBlock* LOOP2= CreateBlockD("loop2", F);

		auto N_MINUS_M = llvm::ConstantInt::get(INT32TYPE,N-M,false);
		builder.CreateCondBr(builder.CreateICmpEQ(KKP1, N_MINUS_M), LOOP2, LOOP1);
		builder.SetInsertPoint(LOOP2);
		KK = builder.CreatePHI(INT32TYPE,2);
		KK->addIncoming(N_MINUS_M,LOOP1);

		KKP1 = builder.CreateAdd(KK,LONG_ONE);
		KK->addIncoming(KKP1, LOOP2);
		MTKK_P = builder.CreateGEP(MT,KK);
		Y = builder.CreateOr(
				builder.CreateAnd(builder.CreateLoad(MTKK_P),UPPER_MASK),
				builder.CreateAnd(builder.CreateLoad(builder.CreateGEP(MT,KKP1)),LOWER_MASK));

		builder.CreateStore(builder.CreateXor(builder.CreateXor(builder.CreateLoad(
				builder.CreateGEP(MT,
						builder.CreateAdd(KK,getInt32(M-N))
				)),builder.CreateLShr(Y,(uint64_t)1)),
				builder.CreateSelect(builder.CreateTrunc(Y, BOOLTYPE),MATRIX_A,LONG_ZERO)),MTKK_P);

		llvm::BasicBlock* END_LOOP= CreateBlockD("endLoop", F);
		auto N_MINUS_1 = llvm::ConstantInt::get(INT32TYPE,N-1,false);
		builder.CreateCondBr(builder.CreateICmpEQ(KKP1, N_MINUS_1), END_LOOP, LOOP2);
		builder.SetInsertPoint(END_LOOP);

		MTKK_P = builder.CreateConstGEP1_64(MT, (uint64_t)(N-1));

		Y = builder.CreateOr(
				builder.CreateAnd(builder.CreateLoad(MTKK_P),UPPER_MASK),
				builder.CreateAnd(builder.CreateLoad(MT),LOWER_MASK));

		builder.CreateStore(builder.CreateXor(builder.CreateXor(builder.CreateLoad(
				builder.CreateConstGEP1_64(MT,(uint64_t)(M-1))),builder.CreateLShr(Y,(uint64_t)1)),
				builder.CreateSelect(builder.CreateTrunc(Y, BOOLTYPE),MATRIX_A,LONG_ZERO)),MTKK_P);
		builder.CreateBr(GEN_NUM);
		builder.SetInsertPoint(GEN_NUM);

		auto IDX2 = builder.CreatePHI(INT32TYPE,2);
		IDX2->addIncoming(LONG_ZERO, END_LOOP);
		IDX2->addIncoming(IDX,BB);

		builder.CreateStore(builder.CreateAdd(IDX2,LONG_ONE), IDX_P);
		Y = builder.CreateLoad(builder.CreateGEP(MT, IDX2));
		Y = builder.CreateXor(Y, builder.CreateLShr(Y, (uint64_t)11));
		Y = builder.CreateXor(Y, builder.CreateAnd(builder.CreateShl(Y, (uint64_t)7),llvm::ConstantInt::get(INT32TYPE, 0x9d2c5680UL, false)));
		Y = builder.CreateXor(Y, builder.CreateAnd(builder.CreateShl(Y, (uint64_t)15),llvm::ConstantInt::get(INT32TYPE, 0xefc60000UL, false)));
		Y = builder.CreateXor(Y, builder.CreateLShr(Y, (uint64_t)18));
		builder.CreateRet(Y);
		this->FinalizeFunctionD(F);
	#undef M
	#undef N
	}
	if(PARENT) builder.SetInsertPoint(PARENT);
	return builder.CreateCall2(F,REAL_MT,REAL_IDX_P);
}
/*
bool RData::conditionalError(llvm::Value* V, String s, PositionID id){
	assert(V);
	if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
		if(C->isOne()){
			error(s,id);
			return true;
		}
		else{
			return false;
		}
	} else {
		auto FUNCT = builder.GetInsertBlock()->getParent();
		auto ERROR_B = CreateBlockD("ERROR", FUNCT);
		auto NO_ERROR_B = CreateBlockD("NO_ERROR", FUNCT);
		builder.CreateCondBr(V, ERROR_B, NO_ERROR_B);
		builder.SetInsertPoint(ERROR_B);
		error(s,id);
		builder.SetInsertPoint(NO_ERROR_B);
		return false;
	}
}*/
void RData::println(String m){
	llvm::SmallVector<llvm::Type*,1> args(1);
	args[0] = C_STRINGTYPE;
	llvm::FunctionType *FT = llvm::FunctionType::get(C_INTTYPE, args, false);
	builder.CreateCall(getExtern("puts", FT, ""),getConstantCString(m));
}
/*
		void RData::error(String s){
			auto CU = getExtern("putchar", &c_intClass, {&c_intClass});
			//std::stringstream ss;
			//ss << s << " at " << id.fileName << ":" << id.lineN << ":" << id.charN << endl;
			for(const auto& a: s)
				builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));

			auto EX = getExtern("exit", &c_intClass, {&c_intClass});
			builder.CreateCall(EX, llvm::ConstantInt::get(c_intClass.type, 1,false));
			builder.CreateUnreachable();
		}*/
inline llvm::Constant* RData::getExtern(String name, const AbstractClass* R, const std::vector<const AbstractClass*>& A, bool varArgs, String lib){
	llvm::SmallVector<llvm::Type*,0> args(A.size());
	for(unsigned i = 0; i<A.size(); i++){
		assert(A[i]);
		assert(A[i]->type);
		args[i] = A[i]->type;
	}
	assert(R);
	assert(R->type);
	llvm::FunctionType *FT = llvm::FunctionType::get(R->type, args, varArgs);
	return getExtern(name, FT, lib);
}
void RData::makeJump(String name, JumpType jump, const Data* val, PositionID id){
	if(name==""){
		if(jump==RETURN){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump==FUNC){
					if(jumps[i]->returnType==nullptr){
						id.error("Cannot return from function that does not allow returns");
					} else if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						assert(jumps[i]->scope);
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
						builder.CreateRetVoid();
					}
					else {
						auto tmp = val->castToV(*this, jumps[i]->returnType, id);
						assert(jumps[i]->scope);
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
						builder.CreateRet(tmp);
					}
					return;
				} else {
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}
				}
				if(i <= 0){
					for(auto a: jumps)
						cerr << a->name << " and " << str(a->toJump) << endl << flush;
					id.compilerError("Error could not find returning block - func");
					exit(1);
				}
			}
		} else if(jump==YIELD){
			id.warning("Generator variable garbage collection / cleanup needs to be implemented");
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump==GENERATOR){
					llvm::BasicBlock* cur = builder.GetInsertBlock();
					assert(jumps[i]->returnType);
					if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, &VOID_DATA));
					}
					else jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, val->castTo(*this, jumps[i]->returnType, id)));
					//assert(jumps[i]->end);
					//builder.CreateBr(jumps[i]->end);//TODO DECREMENT ALL COUNTS BEFORE HERE
					llvm::BasicBlock *RESUME = CreateBlock("postReturn",cur);
					jumps[i]->resumes.push_back(std::pair<llvm::BasicBlock*,llvm::BasicBlock*>(cur,RESUME));
					builder.SetInsertPoint(RESUME);
					return;
				} else {
					//TODO GENERATOR SCOPE GC
					/*
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}*/
				}
				if(i <= 0){
					id.compilerError("Error could not find returning block - gen");
					exit(1);
				}
			}
		} else{
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					builder.CreateBr((jump==BREAK)?(jumps[i]->end):(jumps[i]->start));
					return;
				} else {
					if(jumps[i]->scope){
						for(const auto& dat: jumps[i]->scope->vars){
							decrementCount(*this, id, dat);
						}
					}
				}
				if(i <= 0){
					id.compilerError("Error could not find continue/break block");
					exit(1);
				}
			}
		}
	} else {
		id.compilerError("Named jumps not supported yet");
		exit(1);
	}
}

llvm::Value* RData::phiRecur(std::set<llvm::PHINode*> done, std::vector<LazyLocation*>& V, unsigned idx, llvm::PHINode* target,bool prop){
	assert(target);
	if(!done.insert(target).second) return target;
	bool isSame = true;
	llvm::Value* run=nullptr;
	for(auto bi=target->block_begin(); bi!=target->block_end(); ++bi){
		auto val = target->getIncomingValueForBlock(*bi);
		if(val==target) continue;
		else if(llvm::dyn_cast<llvm::UndefValue>(val)){
			continue;
		} else if(run==val){
			continue;
		} else if(run==nullptr){
			run = val;
			continue;
		} else {
			isSame = false;
			break;
		}
	}
	if(!isSame) return target;
	if(!run){
		bool warned=false;
		for(unsigned i=0; i<V.size(); i++)
		for(auto& a: V[i]->phi){
			if(a.second.first==target){
				a.second.second.warning("Variable "+V[i]->getName()+" undefined");
				warned = true;
				break;
			}
		}
		if(!warned){
			PositionID("#unknown",0,0).warning("Unknown variable undefined");
		}
		run = llvm::UndefValue::get(target->getType());
	}

	assert(target->getType()==run->getType());
	if(!prop){
		for(auto& a: V[idx]->data){
			if(a.second==target){
				a.second = run;
			}
		}
	} else {
		for(unsigned i=idx; i<V.size(); i++)
		for(auto& a: V[i]->data){
			if(a.second==target){
				a.second = run;
			}
		}
	}

	std::vector<llvm::PHINode*> p;
#if defined(LLVM_VERSION_MAJOR) && LLVM_VERSION_MAJOR == 3 && LLVM_VERSION_MINOR <= 4
	for(auto I = target->use_begin(),  E = target->use_end();  I != E; ++I){
#else
	for(auto I = target->user_begin(), E = target->user_end(); I != E; ++I){
#endif
		auto U = (*I);
		if(auto * C = llvm::dyn_cast<llvm::PHINode>(U))
			if(C!=target){
				p.push_back(C);
			}
	}
	target->replaceAllUsesWith(run);
	//TODO add back recursion, but without sigsegv
	//for(auto& a: p) phiRecur(done,V,idx,a,prop);
	target->eraseFromParent();
	return run;
}

llvm::Value* RData::getLastValueOf(std::set<llvm::PHINode*> done,std::vector<LazyLocation*>& V, unsigned idx, llvm::BasicBlock* b, PositionID id){
	assert(b);
	auto ll = V[idx];
	assert(ll);
	auto found = ll->data.find(b);
	if(found!=ll->data.end()){
		auto tmp = builder.GetInsertBlock();
		builder.SetInsertPoint(b);
		auto V = ll->getFastValue(*this,found);
		if(tmp) builder.SetInsertPoint(tmp);
		assert(V);
		assert(V->getType()==ll->type);
		return V;
	} else {
		assert(ll->phi.find(b)==ll->phi.end());
		if(llvm::BasicBlock* prev = b->getUniquePredecessor()){
			auto Va = getLastValueOf(done,V,idx,prev,id);
			ll->data[prev] = Va;
			assert(Va);
			assert(Va->getType()==ll->type);
			return Va;
		} else {

			llvm::pred_iterator PI = pred_begin(b);
			llvm::pred_iterator E = pred_end(b);
			//NO DEFINITION
			if(PI==E){
				id.warning("Variable "+ll->getName()+" undefined");
				return llvm::UndefValue::get(ll->type);
			} else {
				auto tmp = builder.GetInsertBlock();
				builder.SetInsertPoint(b);
				llvm::PHINode* np = CreatePHI(ll->type, 1U/*,ll->name*/);
				if(tmp) builder.SetInsertPoint(tmp);
				ll->data[b] = np;//todo speed up

				for(; PI!=E; ++PI){
					llvm::BasicBlock* me = *PI;
					np->addIncoming(getLastValueOf(done,V,idx,me,id),me);
				}
				llvm::Value* ret=phiRecur(done,V, idx, np,false);
				assert(ret);
				assert(ret->getType()==ll->type);
				return ret;
			}
		}
	}
}

void RData::FinalizeFunction(llvm::Function* f){
	assert(f);
	//llvm::BasicBlock* Parent = builder.GetInsertBlock();
	auto V = flocs.find(f)->second;
	std::set<llvm::PHINode*> done;
	for(unsigned idx = 0; idx < V.size(); ++idx){
		auto ll = V[idx];
		//		ll->phi.
		std::vector<llvm::PHINode*> todo;
		for(std::map<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >::iterator it = ll->phi.begin(); it!=ll->phi.end(); ++it){
			assert(it->second.first);

			auto np = it->second.first;
			auto b = np->getParent();
			//auto tmp = builder.GetInsertBlock();

			llvm::pred_iterator PI = pred_begin(b);
			llvm::pred_iterator E = pred_end(b);
			//NO DEFINITION
			if(PI==E){
				it->second.second.warning("Variable "+ll->getName()+" undefined");
				auto run = llvm::UndefValue::get(ll->type);
				phiRecur(done,V, idx, np, run);
			} else {

				for(; PI!=E; ++PI){
					llvm::BasicBlock* me = *PI;
					np->addIncoming(getLastValueOf(done,V,idx,me,it->second.second),me);
				}
				phiRecur(done,V, idx, np, true);
			}
		}
		if(!ll->used){
			if(llvm::Instruction* u = llvm::dyn_cast<llvm::Instruction>(ll->position)) u->eraseFromParent();
		}
		delete ll;
	}
	//if(Parent) builder.SetInsertPoint(Parent);
	//cerr << "start finalizing function" << endl << flush;
	if(debug){
		f->dump();
		cerr << "ENDPREV" << endl << flush;
	}
	fpm.run(*f);
	flocs.erase(f);
	pred.erase(f);
	if(debug){
		f->dump();
		cerr << endl << flush;
	}
	//f->dump();
	//cerr << endl << flush;
	//cerr << "done finalizing function" << endl << flush;
}

#endif /* RDATAP_HPP_ */
