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
#include "./data/VoidData.hpp"
#include "../operators/Deconstructor.hpp"

inline llvm::Function* RData::getExtern(String name, const AbstractClass* R, const std::vector<const AbstractClass*>& A, bool varArgs, String lib){
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
					if(jumps[i]->returnType->classType==CLASS_VOID){
						if(val->type!=R_VOID && val->getReturnType()->classType!=CLASS_VOID) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, &VOID_DATA));
					}
					else jumps[i]->endings.push_back(std::pair<llvm::BasicBlock*,const Data*>(cur, val->castTo(*this, jumps[i]->returnType, id)));
					builder.CreateBr(jumps[i]->end);//TODO DECREMENT ALL COUNTS BEFORE HERE
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

//TODO do this
llvm::Value* RData::getLastValueOf(LazyLocation* ll, llvm::BasicBlock* b, PositionID id){
	assert(b);
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
			auto V = getLastValueOf(ll,prev,id);
			ll->data[prev] = V;
			assert(V);
			assert(V->getType()==ll->type);
			return V;
		} else {

			llvm::pred_iterator PI = pred_begin(b);
			llvm::pred_iterator E = pred_end(b);
			//NO DEFINITION
			if(PI==E){
				//TODO CAUSE UNDEF ERROR
				id.warning("Variable "+ll->getName()+" undefined");
				return llvm::UndefValue::get(ll->type);
			} else {
				auto tmp = builder.GetInsertBlock();
				builder.SetInsertPoint(b);
				//todo.push_back(b);
				llvm::PHINode* np = CreatePHI(ll->type, 1U/*,ll->name*/);
				if(tmp) builder.SetInsertPoint(tmp);
				ll->data[b] = np;//todo speed up

				for(; PI!=E; ++PI){
					llvm::BasicBlock* me = *PI;
					np->addIncoming(getLastValueOf(ll,me,id),me);
				}
				llvm::Value* ret=np;
				bool isSame = true;
				llvm::Value* run=nullptr;
				for(auto bi=np->block_begin(); bi!=np->block_end(); ++bi){
					auto val = np->getIncomingValueForBlock(*bi);
					if(val==np) continue;
					else if(run==nullptr){
						run = val;
						continue;
					} else if(run==val){
						continue;
					} else {
						isSame = false;
						break;
					}
				}
				if(isSame){
					assert(run);
					ret = run;
					for(auto& a: ll->data){
						if(a.second==np) a.second = run;
					}
					np->replaceAllUsesWith(run);
					np->eraseFromParent();
				} else {
			//		auto toRet = ll->phi.insert(std::pair<llvm::BasicBlock*,std::pair<llvm::PHINode*,PositionID> >
			//			(b, std::pair<llvm::PHINode*,PositionID>(np,PositionID("#recur",0,0))));
				}
				assert(ret);
				assert(ret->getType()==ll->type);
				return ret;
			}
		}
	}
}

void RData::FinalizeFunction(llvm::Function* f){
	//llvm::BasicBlock* Parent = builder.GetInsertBlock();
	for(LazyLocation*& ll: flocs.find(f)->second){
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
				//TODO CAUSE UNDEF ERROR
				it->second.second.warning("Variable "+ll->getName()+" undefined");
				auto run = llvm::UndefValue::get(ll->type);

				for(auto& a: ll->data){
					if(a.second==np) a.second = run;
				}
				np->replaceAllUsesWith(run);
				np->eraseFromParent();
			} else {

				for(; PI!=E; ++PI){
					llvm::BasicBlock* me = *PI;
					np->addIncoming(getLastValueOf(ll,me,it->second.second),me);
				}
				llvm::Value* ret=np;
				bool isSame = true;
				llvm::Value* run=nullptr;
				for(auto bi=np->block_begin(); bi!=np->block_end(); ++bi){
					auto val = np->getIncomingValueForBlock(*bi);
					if(val==np) continue;
					else if(run==nullptr){
						run = val;
						continue;
					} else if(run==val){
						continue;
					} else {
						isSame = false;
						break;
					}
				}
				if(isSame){
					ret = run;

					for(auto& a: ll->data){
						if(a.second==np) a.second = run;
					}
					np->replaceAllUsesWith(run);
					np->eraseFromParent();
				}
			}
//			todo.push_back(it->second.first)
//				recursiveFinalize(ll,it);
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
		cerr << endl << flush;
		cerr << "ENDPREV" << endl << flush;
		fflush(0);
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
/*

		void RData::FinalizeFunction(Function* f,bool debug=false){
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
 */
#endif /* RDATAP_HPP_ */
