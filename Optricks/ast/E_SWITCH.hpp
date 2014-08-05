/*
 * E_SWITCH.hpp
 *
 *  Created on: Jul 29, 2014
 *      Author: Billy
 */

#ifndef E_SWITCH_HPP_
#define E_SWITCH_HPP_

#include "../language/statement/Statement.hpp"
#include "./Declaration.hpp"
#include "./function/E_GEN.hpp"
#include "../language/location/Location.hpp"
#include "../language/class/GeneratorClass.hpp"
#include "../operators/LocalFuncs.hpp"

class E_SWITCH : public ErrorStatement{
	public:
		Statement* toSwitch;
		//Statement* default_case;
		//whether or not localVariable is used
		mutable std::vector< std::pair<Statement*,Statement*> > body;
		E_SWITCH(PositionID id, Statement* var) :
			ErrorStatement(id), toSwitch(var),body(){
		}
		const AbstractClass* getMyClass(RData& r, PositionID id)const override final{
			id.error("Cannot getSelfClass of statement "+str<Token>(getToken())); exit(1);
		}
		void registerClasses() const override final{
			toSwitch->registerClasses();
			//if(default_case) default_case->registerClasses();
			for(auto& a: body){
				if(std::get<0>(a))
				std::get<0>(a)->registerClasses();
				if(std::get<1>(a))
				std::get<1>(a)->registerClasses();
			}
		}
		void registerFunctionPrototype(RData& r) const override final{
			toSwitch->registerFunctionPrototype(r);
			for(auto& a: body){
				if(std::get<0>(a))
				std::get<0>(a)->registerFunctionPrototype(r);
				if(std::get<1>(a))
				std::get<1>(a)->registerFunctionPrototype(r);
			}
		}
		void buildFunction(RData& r) const override final{
			toSwitch->buildFunction(r);
			for(auto& a: body){
				if(std::get<0>(a))
				std::get<0>(a)->buildFunction(r);
				if(std::get<1>(a))
				std::get<1>(a)->buildFunction(r);
			}
		}

		const AbstractClass* getFunctionReturnType(PositionID id, const std::vector<const Evaluatable*>& args, bool isClassMethod)const override{
			id.error("Switch cannot act as function");
			exit(1);
		}
		const AbstractClass* getReturnType() const override{
			return &voidClass;
		}

		void reset() const override final{
			toSwitch->reset();
			for(auto& a: body){
				if(std::get<0>(a))
					std::get<0>(a)->reset();
				if(std::get<1>(a))
					std::get<1>(a)->reset();
			}
		}
		const Data* evaluate(RData& ra) const override final{
			const Data* D = toSwitch->evaluate(ra);
			auto RT = D->getReturnType();

			llvm::ConstantInt* breaks[body.size()];
			int default_idx = -1;
			int last_idx = -1;
			unsigned unique_blocks = 0;
			for(unsigned i=0; i<body.size(); i++)
				if(body[i].second){
					if(body[i].second->getToken()==T_VOID){
						body[i].second=nullptr;
						continue;
					}
					unique_blocks++;
					last_idx = i;
				}

			if(RT->classType==CLASS_CSTRING /*|| RT->classType==CLASS_STRING*/){
				D = getLocalFunction(ra, filePos, "hash", D, NO_TEMPLATE, std::vector<const Evaluatable*>());

				for(unsigned int i=0; i<body.size(); i++){
					if(body[i].first==nullptr){
						breaks[i] = nullptr;
						if(default_idx!=-1) filePos.error("Cannot have two default cases");
						default_idx = i;
					}
					auto M = body[i].first->evaluate(ra);
					if(M->type!=R_STR){
						filePos.error("Cannot have non-constant case");
						return &VOID_DATA;
					}
					StringLiteral* SL = (StringLiteral*)M;
					int hash=0;
					for(char c: SL->value){
						hash*=31;
						hash+=c;
					}
					for(unsigned j=0; j<i; j++){
						if(breaks[j]==nullptr) continue;
						if(breaks[j]==breaks[i]){
							filePos.error("Cannot have duplicate case (idx "+str(i)+" and "+str(j)+")");
							return &VOID_DATA;
						}
					}
					breaks[i] = getInt32(hash);
				}
			} else {
				for(unsigned int i=0; i<body.size(); i++){
					auto CV = body[i].first->evalCastV(ra, RT, filePos);
					if(CV==nullptr){
						breaks[i] = nullptr;
						if(default_idx!=-1) filePos.error("Cannot have two default cases");
						default_idx = i;
					} else if(auto C = llvm::dyn_cast<llvm::Constant>(CV)){
						auto tmp = ra.getIntType(C);
						assert(llvm::dyn_cast<llvm::ConstantInt>(tmp));
						breaks[i] = (llvm::ConstantInt*)tmp;
					} else {
						filePos.error("Cannot have non-constant case");
						return &VOID_DATA;
					}
				}
			}
			llvm::Value* SV = ra.getIntType(D->getValue(ra, filePos));

			llvm::BasicBlock* PARENT = ra.builder.GetInsertBlock();
			auto F = PARENT->getParent();
			return &VOID_DATA;
			/*
			if(unique_blocks==0){
				//do nothing
			} else if(unique_blocks==1){
				llvm::Value* NV;
				//todo enum detection etc
				llvm::Value* EQ;
				if(default_idx!=-1){
					if(last_idx == body.size()-1){
						body[last_idx].second->evaluate(ra);
						return &VOID_DATA;
					} else if(default_idx <= last_idx){
						if(last_idx == body.size()-2){

							body[last_idx].second->evaluate(ra);
							return &VOID_DATA;
						}
					}
				}
				if(breaks[last_idx]==nullptr){
					//TODO MAKE INTO IF!
					body[last_idx].second->evaluate(ra);
				}
				if(auto C = llvm::dyn_cast<llvm::Constant>(NV)){
					//todo check that this is constant equality
					if(C==SV){
						body[last_idx].second->evaluate(ra);
					} else {
						//do nothing
					}
				} else {
					llvm::Value* EQ = ra.builder.CreateICmpEQ(SV, breaks[0]);
					llvm::BasicBlock* block1 = ra.CreateBlockD("block1", F);
					llvm::BasicBlock* merge = ra.CreateBlockD("merge", F);
					ra.builder.CreateCondBr(EQ, block1, merge);
					ra.builder.SetInsertPoint(block1);
					body[0].second->evaluate(ra);
					ra.builder.CreateBr(merge);
					ra.builder.SetInsertPoint(merge);
				}
			} else if(body.size()==2 && default_idx != -1){
				llvm::Value* EQ = ra.builder.CreateICmpEQ(SV, breaks[1-default_idx]);
				llvm::BasicBlock* block1 = ra.CreateBlockD("block1", F);
				llvm::BasicBlock* default_block = ra.CreateBlockD("default", F);
				llvm::BasicBlock* merge = ra.CreateBlockD("merge", F);
				ra.builder.CreateCondBr(EQ, block1, default_block);
				ra.builder.SetInsertPoint(block1);
				body[0].second->evaluate(ra);
				ra.builder.CreateBr(merge);
				ra.builder.SetInsertPoint(merge);
			}
			if(p_index == 1){
				assert(std::get<1>(body.back()));
				std::get<1>(body.back())->evaluate(ra);
				assert(!ra.hadBreak());
				ra.builder.CreateRet(llvm::ConstantPointerNull::get(C_POINTERTYPE));
			} else {
				llvm::Value* args = F->arg_begin();
				args = ra.builder.CreatePtrToInt(args, C_SIZETTYPE);
				llvm::BasicBlock* tmpB = ra.CreateBlockD("block", F);
				llvm::BasicBlock* END = ra.CreateBlockD("end", F);
				Jumpable k("!parallel", LOOP, //NO SCOPE -- force iterable to deconstruct
										nullptr, nullptr, END, NULL);
				ra.addJump(&k);

				for(unsigned int i=1; i<p_index; i++){

					BB = ra.CreateBlockD("entry", F);
					ra.builder.CreateCondBr(ra.builder.CreateICmpULT(args, p_sum[i]),tmpB,BB);
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
			ra.FinalizeFunction(F);

			ra.builder.SetInsertPoint(PARENT);


			if(llvm::isa<llvm::ConstantInt>(p_sum[p_index]) && ((llvm::ConstantInt*)p_sum[p_index])->isOne()){

				auto THREADS = ra.createAlloca(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(pthread_t)));
				ra.createThread(F, ra.builder.CreateIntToPtr(getSizeT(0),C_POINTERTYPE),THREADS);

				//no more threads!
				if(toJoin){
					ra.joinThread(ra.builder.CreateLoad(THREADS));
				}
			} else {

				auto FUNC = PARENT->getParent();
				auto LOOP = ra.CreateBlockD("spawn_threads", FUNC);
				llvm::BasicBlock* JOIN;
				llvm::BasicBlock* DONE;
				if(toJoin) JOIN = ra.CreateBlockD("join_threads", FUNC);
				DONE = ra.CreateBlockD("done_threads", FUNC);
				llvm::Value* alloc;

				auto THREADS = ra.createAllocaa(llvm::IntegerType::get(llvm::getGlobalContext(), 8*sizeof(pthread_t)),
						(toJoin)?ra.builder.CreateIntCast(p_sum[p_index],INT32TYPE,false):(llvm::Value*)getInt32(1));
				if(toJoin)
					alloc = ra.createAlloca(C_POINTERTYPE);

				ra.builder.CreateBr(LOOP);
				ra.builder.SetInsertPoint(LOOP);
				auto PHI = ra.builder.CreatePHI(C_SIZETTYPE,2);
				PHI->addIncoming(getSizeT(0),PARENT);
				auto PHI_P1 = ra.builder.CreateAdd(getSizeT(1),PHI);
				PHI->addIncoming(PHI_P1, LOOP);
				ra.createThread(F, ra.builder.CreateIntToPtr(PHI,C_POINTERTYPE),toJoin?(llvm::Value*)ra.builder.CreateGEP(THREADS,PHI):(llvm::Value*)THREADS);
				ra.builder.CreateCondBr(ra.builder.CreateICmpEQ(PHI_P1, ra.builder.CreateZExtOrTrunc(p_sum[p_index],C_SIZETTYPE)), toJoin?JOIN:DONE, LOOP);

				ra.builder.SetInsertPoint(DONE);
			}
			return &VOID_DATA;
			*/
		}
		void collectReturns(std::vector<const AbstractClass*>& vals, const AbstractClass* const toBe) override final{
			for(auto& a: body){
				if(std::get<1>(a))
				std::get<1>(a)->collectReturns(vals,toBe);
			}
		}
		const Token getToken() const override {
			return T_SWITCH;
		}
};

#endif /* E_SWITCH_HPP_ */
