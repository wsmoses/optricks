/*
 * E_PARALLEL.hpp
 *
 *  Created on: Jul 28, 2014
 *      Author: Billy
 */

#ifndef E_PARALLEL_HPP_
#define E_PARALLEL_HPP_

#include "../language/statement/Statement.hpp"
#include "./Declaration.hpp"
#include "./function/E_GEN.hpp"
#include "../language/location/Location.hpp"
#include "../language/class/GeneratorClass.hpp"
#include "../operators/LocalFuncs.hpp"

class E_PARALLEL : public ErrorStatement{
	public:
		E_VAR localVariable;
		//whether or not localVariable is used
		std::vector< std::pair<Statement*,Statement*> > body;
		bool toJoin; /* if true join (e.g. parallel body) otherwise don't (e.g. spawn) */
		OModule module;
		mutable std::pair<std::map<llvm::Value*,llvm::PHINode*>, llvm::BasicBlock*> closureInfo;
		E_PARALLEL(PositionID id, String name, PositionID var_id, OModule* mod, bool toJ) :
			ErrorStatement(id), localVariable(Resolvable(&module,name,var_id), false),body(),toJoin(toJ),module(mod){
			closureInfo.second = nullptr;
			module.closureInfo = &closureInfo;
		}
		const AbstractClass* getMyClass(PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		void registerClasses() const override final{
			for(auto& a: body){
				std::get<0>(a)->registerClasses();
				if(std::get<1>(a))
				std::get<1>(a)->registerClasses();
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			for(auto& a: body){
				std::get<0>(a)->registerFunctionPrototype(r);
				if(std::get<1>(a))
				std::get<1>(a)->registerFunctionPrototype(r);
			}
		}
		void buildFunction(RData& r) const override final{
			for(auto& a: body){
				std::get<0>(a)->buildFunction(r);
				if(std::get<1>(a))
				std::get<1>(a)->buildFunction(r);
			}
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			id.error("Parallel cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override{
			return &voidClass;
		}

		void reset() const override final{
			for(auto& a: body){
				std::get<0>(a)->reset();
				if(std::get<1>(a))
				std::get<1>(a)->reset();
			}
		}
		const Data* evaluate(RData& ra) const override final{

			llvm::Value* ar[body.size()];
			llvm::Value* p_sum[body.size()+1];
			Statement* p_stat[body.size()+1];
			p_sum[0] = getSizeT(0);
			unsigned p_index=0;
			for(unsigned int i=0; i<body.size(); i++){
				ar[i] = std::get<0>(body[i])->evaluate(ra)->castToV(ra, &c_size_tClass, filePos);
				if(std::get<1>(body[i])!=nullptr){
					p_sum[p_index+1] = ra.builder.CreateAdd(p_sum[p_index], ar[i]);
					p_index++;
					p_stat[p_index] = std::get<1>(body[i]);
				}
			}
			assert(p_index>=1);
			assert(p_sum[p_index]>0);
			//todo force closures
			closureInfo.second = ra.builder.GetInsertBlock();

			auto FT = llvm::FunctionType::get(C_POINTERTYPE, llvm::SmallVector<llvm::Type*,1>(1, C_POINTERTYPE), false);
			String nam = "!parallel";
			llvm::Function *F = ra.CreateFunction(nam,FT, LOCAL_FUNC);
			llvm::BasicBlock* BB = ra.CreateBlockD("entry", F);
			ra.builder.SetInsertPoint(BB);

			llvm::Value* args = F->arg_begin();
			llvm::Instruction* arg_idx = fcast<llvm::Instruction>(ra.builder.CreatePtrToInt(args, INT32TYPE));
			ConstantData cd(arg_idx, &intClass);
			localVariable.pointer.setObject(&cd);
			if(p_index == 1){
				assert(std::get<1>(body.back()));
				std::get<1>(body.back())->evaluate(ra);
				assert(!ra.hadBreak());
				ra.builder.CreateRet(llvm::ConstantPointerNull::get(C_POINTERTYPE));
			} else {
				llvm::BasicBlock* tmpB = ra.CreateBlockD("block", F);
				llvm::BasicBlock* END = ra.CreateBlockD("end", F);
				Jumpable k("!parallel", LOOP, /*TODO NO SCOPE -- force iterable to deconstruct*/
										nullptr, nullptr, END, NULL);
				ra.addJump(&k);

				for(unsigned int i=1; i<p_index; i++){
					BB = ra.CreateBlockD("entry", F);
					ra.builder.CreateCondBr(ra.builder.CreateICmpULT(arg_idx, p_sum[i]),tmpB,BB);
					ra.builder.SetInsertPoint(tmpB);
					if(i==p_index-1)
						tmpB = BB;
					else
						tmpB = ra.CreateBlockD("block", F);
					k.start = tmpB;

					p_stat[i]->evaluate(ra);

					if(!ra.hadBreak())
						ra.builder.CreateBr(END);
					ra.builder.SetInsertPoint(BB);
				}
				k.start = END;
				p_stat[p_index]->evaluate(ra);
				if(!ra.hadBreak())
					ra.builder.CreateBr(END);

				if(ra.popJump()!= &k) error("Did not receive same func jumpable created (k)");

				ra.builder.SetInsertPoint(END);
				ra.builder.CreateRet(llvm::ConstantPointerNull::get(C_POINTERTYPE));
			}

			llvm::Type* PT;
			if(closureInfo.first.size() > 0){
				llvm::SmallVector<llvm::Type*,2> types(1+closureInfo.first.size());
				types[0] = INT32TYPE;
				unsigned idx = 1;
				for(auto& a: closureInfo.first){
					types[idx] = a.first->getType();
					idx++;
				}
				llvm::SmallVector<llvm::Type*,1> NT(1);

				llvm::Instruction* toload = fcast<llvm::Instruction>(ra.builder.CreatePointerCast(args,
						llvm::PointerType::getUnqual(PT = llvm::StructType::get(llvm::getGlobalContext(),types))));
				toload->moveBefore(arg_idx);

				llvm::Instruction* load = fcast<llvm::Instruction>(ra.builder.CreateConstGEP2_32(toload,0,0));
				load->moveBefore(arg_idx);
				load = ra.builder.CreateLoad(load);
				load->moveBefore(arg_idx);
				arg_idx->replaceAllUsesWith(load);

				idx = 1;
				for(auto& a: closureInfo.first){
					llvm::Instruction* load = fcast<llvm::Instruction>(ra.builder.CreateConstGEP2_32(toload,0,idx));
					load->moveBefore(arg_idx);
					load = ra.builder.CreateLoad(load);
					load->moveBefore(arg_idx);
					a.second->replaceAllUsesWith(load);
					idx++;
				}
				arg_idx->eraseFromParent();
			}
			ra.FinalizeFunction(F);

			ra.builder.SetInsertPoint(closureInfo.second);


			if(llvm::isa<llvm::ConstantInt>(p_sum[p_index]) && ((llvm::ConstantInt*)p_sum[p_index])->isOne()){

				auto THREADS = ra.createAlloca(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(pthread_t)));
				llvm::Value* PTR;
				if(closureInfo.first.size()>0){
					llvm::Value* MEM = ra.createAlloca(PT);
					ra.builder.CreateStore(getInt32(0), ra.builder.CreateConstGEP2_32(MEM,0,0));
					unsigned idx = 1;
					for(auto& a: closureInfo.first){
						ra.builder.CreateStore(a.first, ra.builder.CreateConstGEP2_32(MEM,0,idx));
						idx++;
					}
					PTR = ra.builder.CreatePointerCast(MEM, C_POINTERTYPE);
				} else PTR = ra.builder.CreateIntToPtr(getSizeT(0),C_POINTERTYPE);
				ra.createThread(F, PTR,THREADS);

				//no more threads!
				if(toJoin){
					ra.joinThread(ra.builder.CreateLoad(THREADS));
				}
			} else {

				auto FUNC = closureInfo.second->getParent();
				auto LOOP = ra.CreateBlockD("spawn_threads", FUNC);
				llvm::BasicBlock* JOIN;
				llvm::BasicBlock* DONE;
				if(toJoin) JOIN = ra.CreateBlockD("join_threads", FUNC);
				DONE = ra.CreateBlockD("done_threads", FUNC);
				llvm::Value* alloc;
				llvm::Value* NUM_THREADS = (toJoin || closureInfo.first.size() > 0) ?ra.builder.CreateIntCast(p_sum[p_index],INT32TYPE,false):getInt32(1);

				auto THREADS = ra.createAlloca(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(pthread_t)),
						NUM_THREADS);
				if(toJoin)
					alloc = ra.createAlloca(C_POINTERTYPE);
				llvm::Value* MEM;
				if(closureInfo.first.size()>0)
					MEM = ra.createAlloca(PT, NUM_THREADS);

				ra.builder.CreateBr(LOOP);
				ra.builder.SetInsertPoint(LOOP);
				llvm::PHINode* PHI = ra.builder.CreatePHI((closureInfo.first.size()>0)?INT32TYPE:C_SIZETTYPE,2);
				PHI->addIncoming((closureInfo.first.size()>0)?getInt32(0):getSizeT(0),closureInfo.second);
				auto PHI_P1 = ra.builder.CreateAdd((closureInfo.first.size()>0)?getInt32(1):getSizeT(1),PHI);
				PHI->addIncoming(PHI_P1, LOOP);

				llvm::Value* PTR;
				if(closureInfo.first.size()>0){
					llvm::SmallVector<llvm::Value*,2> VALS(2);
					VALS[0] = PHI;
					VALS[1] = getInt32(0);
					ra.builder.CreateStore(PHI, ra.builder.CreateInBoundsGEP(MEM,VALS));
					unsigned idx = 1;
					for(auto& a: closureInfo.first){
						VALS[1] = getInt32(idx);
						ra.builder.CreateStore(a.first, ra.builder.CreateInBoundsGEP(MEM,VALS));
						idx++;
					}
					PTR = ra.builder.CreatePointerCast(ra.builder.CreateGEP(MEM,PHI), C_POINTERTYPE);
				} else PTR = ra.builder.CreateIntToPtr(PHI,C_POINTERTYPE);

				ra.createThread(F, PTR,toJoin?(llvm::Value*)ra.builder.CreateGEP(THREADS,PHI):(llvm::Value*)THREADS);
				ra.builder.CreateCondBr(ra.builder.CreateICmpEQ(PHI_P1, ra.builder.CreateZExtOrTrunc(p_sum[p_index],PHI_P1->getType())), toJoin?JOIN:DONE, LOOP);

				if(toJoin){
					ra.builder.SetInsertPoint(JOIN);
					auto PHI = ra.builder.CreatePHI(INT32TYPE,2);
					PHI->addIncoming(getInt32(0),LOOP);
					auto PHI_P1 = ra.builder.CreateAdd(getInt32(1),PHI);
					PHI->addIncoming(PHI_P1, JOIN);
					ra.joinThread(ra.builder.CreateLoad(ra.builder.CreateGEP(THREADS,PHI)),alloc);
					ra.builder.CreateCondBr(ra.builder.CreateICmpEQ(PHI_P1, ra.builder.CreateZExtOrTrunc(p_sum[p_index],INT32TYPE)), DONE, JOIN);
				}

				ra.builder.SetInsertPoint(DONE);
			}
			return &VOID_DATA;
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
		}
		const Token getToken() const override {
			return T_PARALLEL;
		}
};

#endif /* E_PARALLEL_HPP_ */
