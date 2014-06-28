/*
 * LocalFuncs.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef LOCALFUNCS_HPP_
#define LOCALFUNCS_HPP_

#include "../language/class/AbstractClass.hpp"
#include "./Binary.hpp"

bool hasLocalFunction(String s, const AbstractClass* cc){
	if(s=="hash") return true;
	switch(cc->classType){
	case CLASS_USER:{
		auto uc = (const UserClass*)cc;
		return uc->hasLocalFunction(s);
	}
	case CLASS_HASHMAP:
	case CLASS_ARRAY:{
		if(s=="isEmpty") return true;
		else return false;
	}
	case CLASS_PRIORITYQUEUE:{
		if(s=="poll") return true;
		if(s=="isEmpty") return true;
		else return false;
	}
	default:
		return false;
	}
}
const AbstractClass* getLocalFunctionReturnType(PositionID id, String s, const AbstractClass* cc, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v){
	if(s=="hash" && v.size()==0)
		return &intClass;
	switch(cc->classType){
	case CLASS_USER:{
		auto uc = (const UserClass*)cc;
		auto lf = uc->getLocalFunction(id, s, t_args, v);
		if(lf==nullptr) return &voidClass;
		return lf->getSingleProto()->returnType;
	}
	case CLASS_HASHMAP:
	case CLASS_ARRAY:{
		if(s=="isEmpty" && v.size()==0) return &boolClass;
		else break;
	}
	case CLASS_PRIORITYQUEUE:{
		auto uc = (const PriorityQueueClass*)cc;
		if(s=="poll" && v.size()==0) return uc->inner;
		if(s=="isEmpty" && v.size()==0) return &boolClass;
		else break;
	}
	default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}

const Data* getLocalFunction(RData& r, PositionID id, String s, const Data* inst, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v){
	assert(inst);
	const AbstractClass* cc = inst->getReturnType();
	if(s=="hash" && v.size()==0){

		switch(cc->classType){
		case CLASS_INT:
		case CLASS_BOOL:
		case CLASS_CHAR:
			return new ConstantData(r.builder.CreateZExtOrTrunc(inst->getValue(r, id), intClass.type),&intClass);
		case CLASS_FLOAT:
			//TODO check that all NaN hash to same value...
			return new ConstantData(r.builder.CreateTruncOrBitCast(inst->getValue(r, id), intClass.type),&intClass);
		case CLASS_NULL:
			return new ConstantData(getInt32(0),&intClass);
		case CLASS_ARRAY:
		case CLASS_CPOINTER:
		case CLASS_FUNC:
		case CLASS_HASHMAP:
			return new ConstantData(r.builder.CreatePtrToInt(inst->getValue(r, id), intClass.type),&intClass);
		case CLASS_USER:{
			auto uc = (const UserClass*)cc;
			auto lf = uc->getLocalFunction(id, s, t_args, v);
			if(lf==nullptr) return &VOID_DATA;
			return lf->callFunction(r,id, v, inst);
		}
		default: break;
		}
	}
	switch(cc->classType){
		case CLASS_USER:{
			auto uc = (const UserClass*)cc;
			auto lf = uc->getLocalFunction(id, s, t_args, v);
			if(lf==nullptr) return &VOID_DATA;
			return lf->callFunction(r,id, v, inst);
		}
		case CLASS_HASHMAP:{
			//auto hm = (const HashMapClass*)cc;
			if(s=="isEmpty" && v.size()==0){
				auto V = inst->getValue(r, id);
				//TODO assert non null
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);
				return new ConstantData(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), &boolClass);
			}
			break;
		}
		case CLASS_ARRAY:{
			if(s=="isEmpty" && v.size()==0){
				auto V = inst->getValue(r, id);
				//TODO assert non null
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);
				return new ConstantData(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), &boolClass);
			}
			break;
		}
		case CLASS_PRIORITYQUEUE:{
			if(s=="poll" && v.size()==0){

				PriorityQueueClass* AC = (PriorityQueueClass*) cc;
				static std::map<llvm::Type*,llvm::Function*> MAP;
				llvm::Function* F;
				auto find = MAP.find(AC->type);
				if(find==MAP.end()){
					llvm::SmallVector<llvm::Type*,1> ar(1);
					ar[0] = AC->type;
					F = r.CreateFunctionD("_opt"+AC->getName()+".poll", llvm::FunctionType::get(AC->inner->type, ar, false), LOCAL_FUNC);

					llvm::BasicBlock* Parent = r.builder.GetInsertBlock();
					llvm::BasicBlock* BB = r.CreateBlockD("entry", F);
					r.builder.SetInsertPoint(BB);
					llvm::Value* V;
					{
						llvm::Function::arg_iterator AI = F->arg_begin();
						AI->setName("pq");
						V = AI;
					}
					//TODO assert non null
					auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
					llvm::Value* LENGTH = r.builder.CreateLoad(LENGTH_P);


					auto STARTT = r.builder.GetInsertBlock();
					auto FUNC = STARTT->getParent();
					auto ERROR_B = r.CreateBlockD("ERROR", FUNC);
					auto NO_ERROR_B = r.CreateBlockD("NO_ERROR", FUNC);
					r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), ERROR_B, NO_ERROR_B);
					r.builder.SetInsertPoint(ERROR_B);
					llvm::SmallVector<llvm::Type*,1> t_args(1);
					t_args[0] = C_STRINGTYPE;
					llvm::SmallVector<llvm::Value*,4> c_args(4);
					c_args[0] = r.getConstantCString("PriorityQueue poll() called on empty list at %s:%d:%d\n");
					c_args[1] = r.getConstantCString(id.fileName);
					c_args[2] = getInt32(id.lineN);
					c_args[3] = getInt32(id.charN);
					r.builder.CreateCall(r.getExtern("printf", llvm::FunctionType::get(c_intClass.type, t_args,true)), c_args);
					r.error("");
					r.builder.SetInsertPoint(NO_ERROR_B);

					LENGTH = r.builder.CreateSub(LENGTH, getInt32(1));
					r.builder.CreateStore(LENGTH,LENGTH_P);
					const auto DATA_P = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
					const auto toRet = r.builder.CreateLoad(DATA_P);
					auto IC = AC->inner;
					//const auto toRetD = new ConstantData(toRet, IC);
					const auto LAST = r.builder.CreateLoad(r.builder.CreateGEP(DATA_P,LENGTH));
					ConstantData OBJ_LAST(LAST, IC);

					const auto HALF = r.builder.CreateLShr(LENGTH,getInt32(1));
					auto LOOP = r.CreateBlockD("loop", FUNC);
					auto IN_BOUNDS = r.CreateBlockD("in_bounds", FUNC);
					auto SWAP = r.CreateBlockD("swap", FUNC);
					auto SWAP2 = r.CreateBlockD("not_break_swap", FUNC);
					auto NORMAL = r.CreateBlockD("NORMAL", FUNC);
					auto not_break = r.CreateBlockD("not_break", FUNC);
					auto END = r.CreateBlockD("end", FUNC);
					auto END2 = r.CreateBlockD("end2", FUNC);
					r.builder.CreateCondBr(r.builder.CreateICmpULT(getInt32(0),HALF), LOOP, END2);

					r.builder.SetInsertPoint(END2);
					r.builder.CreateRet(toRet);

					r.builder.SetInsertPoint(END);
					auto END_K = r.builder.CreatePHI(intClass.type,5);

					r.builder.SetInsertPoint(LOOP);
					auto K = r.builder.CreatePHI(intClass.type,3);
					K->addIncoming(getInt32(0),NO_ERROR_B);

					auto CHILD = r.builder.CreateAdd(r.builder.CreateShl(K,1),getInt32(1));
					auto obj_c = r.builder.CreateLoad(r.builder.CreateGEP(DATA_P,CHILD));
					ConstantData OBJ_C(obj_c, IC);

					auto RIGHT = r.builder.CreateAdd(CHILD,getInt32(1));

					r.builder.CreateCondBr(r.builder.CreateICmpULT(RIGHT,LENGTH), IN_BOUNDS, NORMAL);

					r.builder.SetInsertPoint(IN_BOUNDS);
					auto obj_right = r.builder.CreateLoad(r.builder.CreateGEP(DATA_P,RIGHT));
					ConstantData OBJ_RIGHT(obj_right, IC);
					auto cmp = getBinop(r, id, &OBJ_RIGHT, &OBJ_C, "<")->getValue(r, id);
					r.builder.CreateCondBr(cmp, SWAP, NORMAL);

					r.builder.SetInsertPoint(SWAP);

					r.builder.CreateCondBr(getBinop(r, id, &OBJ_LAST, &OBJ_RIGHT, "<")->getValue(r, id), END, SWAP2);
					END_K->addIncoming(K,r.builder.GetInsertBlock());
					r.builder.SetInsertPoint(SWAP2);

					r.builder.CreateStore(obj_right, r.builder.CreateGEP(DATA_P,K));
					K->addIncoming(RIGHT, SWAP2);
					END_K->addIncoming(RIGHT, SWAP2);
					r.builder.CreateCondBr(r.builder.CreateICmpULT(RIGHT, HALF), LOOP, END);

					r.builder.SetInsertPoint(NORMAL);

					r.builder.CreateCondBr(getBinop(r, id, &OBJ_LAST, &OBJ_C, "<")->getValue(r, id), END, not_break);
					END_K->addIncoming(K,r.builder.GetInsertBlock());
					r.builder.SetInsertPoint(not_break);

					r.builder.CreateStore(obj_c, r.builder.CreateGEP(DATA_P,K));
					K->addIncoming(CHILD, not_break);
					END_K->addIncoming(CHILD, not_break);
					r.builder.CreateCondBr(r.builder.CreateICmpULT(CHILD, HALF), LOOP, END);

					r.builder.SetInsertPoint(END);
					r.builder.CreateStore(LAST, r.builder.CreateGEP(DATA_P,END_K));
					r.builder.CreateRet(toRet);
					r.FinalizeFunctionD(F);
					if(Parent) r.builder.SetInsertPoint( Parent );
					MAP.insert(std::pair<llvm::Type*,llvm::Function*>(AC->type,F));
				} else F = find->second;
				auto V = inst->getValue(r, id);
				assert(V);
				return new ConstantData(r.builder.CreateCall(F, V), AC->inner);
			}
			if(s=="isEmpty" && v.size()==0){
				auto V = inst->getValue(r, id);
				//TODO assert non null
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);
				return new ConstantData(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), &boolClass);
			}
			break;
		}
		default: break;
	}
	id.compilerError("Local functions not implemented yet  "+cc->getName());
	exit(1);
}



#endif /* LOCALFUNCS_HPP_ */
