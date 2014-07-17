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
		if(s=="containsKey") return true;
		else if(s=="containsValue") return true;
		else if(s=="setDefault") return true;
		else if(s=="contains") return true;
		else if(s=="remove") return true;
		else if(s=="isEmpty") return true;
		else if(s=="clear") return true;
		else return false;
	case CLASS_ARRAY:{
		if(s=="add") return true;
		else if(s=="shuffle") return true;
		else if(s=="reverse") return true;
		else if(s=="replace") return true;
		else if(s=="binarySearch") return true;
		else if(s=="pop") return true;
		else if(s=="sort") return true;
		else if(s=="count") return true;
		else if(s=="indexOf") return true;
		else if(s=="lastIndexOf") return true;
		else if(s=="swap") return true;
		else if(s=="trim") return true;
		else if(s=="ensureCapacity") return true;
		else if(s=="contains") return true;
		else if(s=="remove") return true;
		else if(s=="isEmpty") return true;
		else if(s=="clear") return true;
		else return false;
	}
	case CLASS_PRIORITYQUEUE:{
		if(s=="peek") return true;
		else if(s=="poll") return true;
		else if(s=="indexOf") return true;
		else if(s=="lastIndexOf") return true;
		else if(s=="trim") return true;
		else if(s=="ensureCapacity") return true;
		else if(s=="contains") return true;
		else if(s=="remove") return true;
		else if(s=="isEmpty") return true;
		else if(s=="clear") return true;
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
		return uc->getLocalFunctionReturnType(id, s, t_args, v);
	}
	case CLASS_HASHMAP:{
		if(s=="isEmpty" && v.size()==0)
			return &boolClass;
		else break;
	}
	case CLASS_ARRAY:{
		auto AC = (const ArrayClass*)cc;
		if(s=="add" && v.size()==2 && v[0]->hasCastValue(&intClass) && v[1]->hasCastValue(AC->inner))
			/* adds value at index, shifting things right if needed */
			return &voidClass;
		else if(s=="shuffle" && v.size()==0)
			/* shuffles in place*/
			return &voidClass;
		else if(s=="reverse" && v.size()==0)
			/* reverses in place */
			return &voidClass;
		else if(s=="replace" && v.size()==2 && v[0]->hasCastValue(AC->inner) && v[1]->hasCastValue(AC->inner))
			/* # of replacements*/
			return &intClass;
		else if(s=="binarySearch" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* Idx where found*/
			return &intClass;
		else if(s=="pop" && ( v.size()==0 || (v.size()==1 && v[0]->hasCastValue(&intClass)) ) )
			/* Element found at idx (end if not specified) */
			return AC->inner;
		else if(s=="sort" && ( v.size()==0 || (v.size()==1 && v[0]->hasCastValue(&boolClass)) ) )
			/* Sorts naturally (reverse if bool is true, default false) */
			return &voidClass;
		else if(s=="count" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* Idx where found or -1 */
			return &intClass;
		else if(s=="indexOf" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* Idx where found or -1 */
			return &intClass;
		else if(s=="lastIndexOf" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* Idx where found or -1*/
			return &intClass;
		else if(s=="swap" && v.size()==2 && v[0]->hasCastValue(&intClass) && v[1]->hasCastValue(&intClass))
			/* swaps elems at idx1 and idx2*/
			return &voidClass;
		else if(s=="trim" && v.size()==0)
			/* forces alloc to be size of used memory*/
			return &voidClass;
		else if(s=="ensureCapacity" && v.size()==1 && v[0]->hasCastValue(&intClass))
			/* If memory is smaller. grow to size*/
			return &voidClass;
		else if(s=="contains" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* whether it contains */
			return &boolClass;
		else if(s=="remove" && v.size()==1 && v[0]->hasCastValue(AC->inner))
			/* Idx where removed or -1 if not present */
			return &intClass;
		else if(s=="isEmpty" && v.size()==0)
			/* size==0 */
			return &boolClass;
		else if(s=="clear" && v.size()==0)
			/* Empties array / decrements count / does not realloc */
			return &voidClass;
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
		case CLASS_BOOL:
		case CLASS_CHAR:
			return new ConstantData(r.builder.CreateZExt(inst->getValue(r, id), intClass.type),&intClass);
		//TODO XOR REMAINING BITS?
		case CLASS_INT:
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
			return uc->callLocalFunction(r, id, s, t_args, v, inst);
		}
		default: break;
		}
	}
	switch(cc->classType){
		case CLASS_USER:{
			auto uc = (const UserClass*)cc;
			return uc->callLocalFunction(r, id, s, t_args, v,inst);
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
			//TODO assert non null
			auto AC = (const ArrayClass*)cc;
			auto V = inst->getValue(r, id);

			if(s=="add" && v.size()==2 && v[0]->hasCastValue(&intClass) && v[1]->hasCastValue(AC->inner)) {
				/* adds value at index, shifting things right if needed */
				//TODO
				return &voidClass;
			} else if(s=="shuffle" && v.size()==0) {
				/* shuffles in place*/
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpULT(LENGTH, getInt32(2)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				auto lenM1 = r.builder.CreateSub(LENGTH, getInt32(1));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);

				auto MAX = r.builder.CreateSub(lenM1, IDX);
				auto J = r.builder.CreateAdd(r.randInt(MAX),IDX);

				/* basic alg *
				 *  for(int i=0; ;){
				 *  	int j = randInt(array.length-1-i)+i
				 *  	array.swap(i,j);
				 *  	i++;
				 *  	if(i==array.length-1) break;
				 *  }
				 */

				auto LEFT_P = r.builder.CreateGEP(DATA, IDX);
				auto LEFT = r.builder.CreateLoad(LEFT_P);
				auto RIGHT_P = r.builder.CreateGEP(DATA, J);
				auto RIGHT = r.builder.CreateLoad(RIGHT_P);
				r.builder.CreateStore(RIGHT, LEFT_P);
				r.builder.CreateStore(LEFT, RIGHT_P);

				auto IDX_P1 = r.builder.CreateAdd(IDX, getInt32(1));
				IDX->addIncoming(IDX_P1, r.builder.GetInsertBlock());

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(lenM1, IDX_P1), DONE, TO_SEARCH);

				r.builder.SetInsertPoint(DONE);
				return &VOID_DATA;
			} else if(s=="reverse" && v.size()==0) {
				/* reverses in place */
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpULT(LENGTH, getInt32(2)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				auto lenM1 = r.builder.CreateSub(LENGTH, getInt32(1));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);

				auto IDX2 = r.builder.CreatePHI(intClass.type, 2);
				IDX2->addIncoming(lenM1, TO_LOAD);

				//auto IDXM1 = r.builder.CreateSub(IDX, getInt32(1));
				auto LEFT_P = r.builder.CreateGEP(DATA, IDX);
				auto LEFT = r.builder.CreateLoad(LEFT_P);
				auto RIGHT_P = r.builder.CreateGEP(DATA, IDX2);
				auto RIGHT = r.builder.CreateLoad(RIGHT_P);
				r.builder.CreateStore(RIGHT, LEFT_P);
				r.builder.CreateStore(LEFT, RIGHT_P);

				auto IDX_P1 = r.builder.CreateAdd(IDX, getInt32(1));
				IDX->addIncoming(IDX_P1, TO_SEARCH);

				auto IDX2_M1 = r.builder.CreateSub(IDX2, getInt32(1));
				IDX2->addIncoming(IDX2_M1, TO_SEARCH);

				r.builder.CreateCondBr(r.builder.CreateICmpUGT(IDX2_M1, IDX_P1), TO_SEARCH, DONE);

				r.builder.SetInsertPoint(DONE);
				return &VOID_DATA;
			} else if(s=="replace" && v.size()==2 && v[0]->hasCastValue(AC->inner) && v[1]->hasCastValue(AC->inner)) {
				/* # of replacements*/

				auto TO_REPLACE = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				auto REPLACE_WITH = v[1]->evaluate(r)->castTo(r, AC->inner, id)->getValue(r, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto INC_COUNT = r.CreateBlockD("inc_count", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,4);
				RET_V->addIncoming(getInt32(0), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);
				auto COUNT = r.builder.CreatePHI(intClass.type, 2);
				COUNT->addIncoming(getInt32(0), TO_LOAD);
				auto FOUND_LOC = r.builder.CreateGEP(DATA, IDX);
				StandardLocation sl(FOUND_LOC);
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_REPLACE, "==")->getValue(r, id);

				r.builder.CreateCondBr(cmp, INC_COUNT, NEXT);

				r.builder.SetInsertPoint(INC_COUNT);
				decrementCount(r, id, &ld);
				r.builder.CreateStore(REPLACE_WITH, FOUND_LOC);
				auto COUNT_P1 = r.builder.CreateAdd(COUNT, getInt32(1));
				r.builder.CreateBr(NEXT);

				r.builder.SetInsertPoint(NEXT);

				auto COUNT_N = r.builder.CreatePHI(intClass.type, 2);
				COUNT_N->addIncoming(COUNT, TO_SEARCH);
				COUNT_N->addIncoming(COUNT_P1, INC_COUNT);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_SEARCH);
				RET_V->addIncoming(COUNT_N, NEXT);
				IDX->addIncoming(IDX2, NEXT);
				COUNT->addIncoming(COUNT_N, NEXT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);

			} else if(s=="binarySearch" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* Assume sorted, Idx where found of -n-1 if not found*/
				auto TO_CHECK = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_LOOP = r.CreateBlockD("to_loop", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto LESS_B = r.CreateBlockD("less", FUNC);
				auto BIG_B = r.CreateBlockD("big", FUNC);
				auto END_LOOP = r.CreateBlockD("end_loop", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,3);
				RET_V->addIncoming(getInt32(-1), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				auto lenM1 = r.builder.CreateSub(LENGTH, getInt32(1));
				r.builder.CreateBr(TO_LOOP);

				r.builder.SetInsertPoint(TO_LOOP);
				auto LOW = r.builder.CreatePHI(intClass.type, 3, "low");
				LOW->addIncoming(getInt32(0), TO_LOAD);
				auto HI = r.builder.CreatePHI(intClass.type, 3, "hi");
				HI->addIncoming(lenM1, TO_LOAD);
				auto MID = r.builder.CreateUDiv(r.builder.CreateAdd(LOW, HI),getInt32(2), "mid");

				StandardLocation sl(r.builder.CreateGEP(DATA, MID));
				LocationData ld(&sl, AC->inner);

				auto cmp = getBinop(r, id, &ld, TO_CHECK, "==")->getValue(r, id);
				r.builder.CreateCondBr(cmp, DONE, NEXT);
				RET_V->addIncoming(MID, TO_LOOP);

				r.builder.SetInsertPoint(NEXT);
				auto cmp2 = getBinop(r, id, &ld, TO_CHECK, "<")->getValue(r, id);
				r.builder.CreateCondBr(cmp2, LESS_B, BIG_B);

				r.builder.SetInsertPoint(END_LOOP);
				auto END_MID = r.builder.CreatePHI(intClass.type, 2);

				r.builder.SetInsertPoint(LESS_B);
				auto MP1 = r.builder.CreateAdd(MID, getInt32(1));
				LOW->addIncoming(MP1, LESS_B);
				HI->addIncoming(HI, LESS_B);
				END_MID->addIncoming(MP1, LESS_B);
				r.builder.CreateCondBr(r.builder.CreateICmpULE(MP1, HI), TO_LOOP, END_LOOP);

				r.builder.SetInsertPoint(BIG_B);
				LOW->addIncoming(LOW, BIG_B);
				auto MID_M1 = r.builder.CreateSub(MID, getInt32(1));
				HI->addIncoming(MID_M1, BIG_B);
				END_MID->addIncoming(MID, BIG_B);
				r.builder.CreateCondBr(r.builder.CreateICmpSLE(LOW, MID_M1), TO_LOOP, END_LOOP);

				r.builder.SetInsertPoint(END_LOOP);
				RET_V->addIncoming(r.builder.CreateSub(getInt32(-1),END_MID), END_LOOP);
				r.builder.CreateBr(DONE);
				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);
			} else if(s=="pop" && ( v.size()==0 || (v.size()==1 && v[0]->hasCastValue(&intClass)) ) ){
				/* Element found at idx (end if not specified) */
				if(v.size()==0){
					auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
					const auto LENGTH = r.builder.CreateLoad(LENGTH_P);

					auto STARTT = r.builder.GetInsertBlock();
					auto FUNC = STARTT->getParent();
					auto IDX_TOO_BIG = r.CreateBlockD("error", FUNC);
					auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
					auto DONE = r.CreateBlockD("done", FUNC);

					r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), IDX_TOO_BIG, TO_LOAD);

					r.builder.SetInsertPoint(IDX_TOO_BIG);
					r.error("Illegal array index %d in %d", id, {getInt32(0), LENGTH});

					r.builder.SetInsertPoint(DONE);
					auto RET_V = r.builder.CreatePHI(intClass.type,4);
					RET_V->addIncoming(getInt32(-1), STARTT);

					r.builder.SetInsertPoint(TO_LOAD);
					auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
					auto lenM1 = r.builder.CreateSub(LENGTH,getInt32(1));
					auto TO_RETURN = new ConstantData(r.builder.CreateLoad(r.builder.CreateGEP(DATA,
							lenM1)), AC->inner);
					//decrementCount(r, id, TO_RETURN); do not dec, returning
					r.builder.CreateStore(lenM1,LENGTH_P);
					return TO_RETURN;
				} else{
					auto REMOVE_IDX = v[0]->evaluate(r)->castToV(r, &intClass, id);
					auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
					const auto LENGTH = r.builder.CreateLoad(LENGTH_P);

					auto STARTT = r.builder.GetInsertBlock();
					auto FUNC = STARTT->getParent();
					auto IDX_TOO_BIG = r.CreateBlockD("error", FUNC);
					auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
					auto SHIFT_LEFT = r.CreateBlockD("shift_left", FUNC);
					auto DONE = r.CreateBlockD("done", FUNC);

					r.builder.CreateCondBr(r.builder.CreateICmpULT(REMOVE_IDX, LENGTH), TO_LOAD, IDX_TOO_BIG);

					r.builder.SetInsertPoint(IDX_TOO_BIG);
					r.error("Illegal array index %d in %d", id, {REMOVE_IDX, LENGTH});

					r.builder.SetInsertPoint(TO_LOAD);
					auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
					auto TO_RETURN = new ConstantData(r.builder.CreateLoad(r.builder.CreateGEP(DATA, REMOVE_IDX)), AC->inner);
					//decrementCount(r, id, TO_RETURN); returning do not dec
					auto lenM1 = r.builder.CreateSub(LENGTH,getInt32(1));
					r.builder.CreateStore(lenM1,LENGTH_P);
					r.builder.CreateCondBr(r.builder.CreateICmpEQ(REMOVE_IDX, lenM1), DONE, SHIFT_LEFT);

					r.builder.SetInsertPoint(SHIFT_LEFT);
					auto toMove = r.builder.CreatePHI(intClass.type, 2);
					toMove->addIncoming(REMOVE_IDX, TO_LOAD);
					auto fromMove = r.builder.CreateAdd(toMove, getInt32(1));
					toMove->addIncoming(fromMove, SHIFT_LEFT);
					r.builder.CreateStore(r.builder.CreateLoad(r.builder.CreateGEP(DATA, fromMove)), r.builder.CreateGEP(DATA, toMove));
					r.builder.CreateCondBr(r.builder.CreateICmpEQ(fromMove, lenM1), DONE, SHIFT_LEFT);

					r.builder.SetInsertPoint(DONE);
					return TO_RETURN;

				}
			} else if(s=="sort" && ( v.size()==0 || (v.size()==1 && v[0]->hasCastValue(&boolClass)) ) ){
				/* Sorts naturally (reverse if bool is true, default false) */
				//TODO
				return &voidClass;
			} else if(s=="count" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* Number of occurences*/

				auto TO_CHECK = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto INC_COUNT = r.CreateBlockD("inc_count", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,4);
				RET_V->addIncoming(getInt32(0), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);
				auto COUNT = r.builder.CreatePHI(intClass.type, 2);
				COUNT->addIncoming(getInt32(0), TO_LOAD);
				StandardLocation sl(r.builder.CreateGEP(DATA, IDX));
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_CHECK, "==")->getValue(r, id);

				r.builder.CreateCondBr(cmp, INC_COUNT, NEXT);

				r.builder.SetInsertPoint(INC_COUNT);
				auto COUNT_P1 = r.builder.CreateAdd(COUNT, getInt32(1));
				r.builder.CreateBr(NEXT);

				r.builder.SetInsertPoint(NEXT);

				auto COUNT_N = r.builder.CreatePHI(intClass.type, 2);
				COUNT_N->addIncoming(COUNT, TO_SEARCH);
				COUNT_N->addIncoming(COUNT_P1, INC_COUNT);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_SEARCH);
				RET_V->addIncoming(COUNT_N, NEXT);
				IDX->addIncoming(IDX2, NEXT);
				COUNT->addIncoming(COUNT_N, NEXT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);

			} else if(s=="indexOf" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* Idx where found or -1 */

				auto TO_CHECK = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,4);
				RET_V->addIncoming(getInt32(-1), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);
				StandardLocation sl(r.builder.CreateGEP(DATA, IDX));
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_CHECK, "==")->getValue(r, id);

				r.builder.CreateCondBr(cmp, DONE, NEXT);
				RET_V->addIncoming(IDX, TO_SEARCH);

				r.builder.SetInsertPoint(NEXT);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_SEARCH);
				RET_V->addIncoming(getInt32(-1), NEXT);
				IDX->addIncoming(IDX2, NEXT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);
			} else if(s=="lastIndexOf" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* Idx where found or -1*/

				auto TO_CHECK = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,4);
				RET_V->addIncoming(getInt32(-1), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(LENGTH, TO_LOAD);
				auto IDXM1 = r.builder.CreateSub(IDX, getInt32(1));
				StandardLocation sl(r.builder.CreateGEP(DATA, IDXM1));
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_CHECK, "==")->getValue(r, id);

				r.builder.CreateCondBr(cmp, DONE, NEXT);
				RET_V->addIncoming(IDXM1, TO_SEARCH);

				r.builder.SetInsertPoint(NEXT);
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(IDXM1, getInt32(0)), DONE, TO_SEARCH);
				RET_V->addIncoming(getInt32(-1), NEXT);
				IDX->addIncoming(IDXM1, NEXT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);

			} else if(s=="swap" && v.size()==2 && v[0]->hasCastValue(&intClass) && v[1]->hasCastValue(&intClass)){
				auto IDX1 = v[0]->evaluate(r)->castToV(r, &intClass, id);
				auto IDX2 = v[1]->evaluate(r)->castToV(r, &intClass, id);
				auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));
				auto MAX = r.builder.CreateSelect(r.builder.CreateICmpUGT(IDX1, IDX2), IDX1, IDX2);

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();

				auto IDX_TOO_BIG = r.CreateBlockD("idx_too_big", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpULT(MAX, LENGTH), DONE, IDX_TOO_BIG);

				r.builder.SetInsertPoint(IDX_TOO_BIG);
				r.error("Illegal array index %d in %d", id, {MAX, LENGTH});

				r.builder.SetInsertPoint(DONE);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				auto p1 = r.builder.CreateGEP(DATA, IDX1);
				auto d1 = r.builder.CreateLoad(p1);
				auto p2 = r.builder.CreateGEP(DATA, IDX2);
				auto d2 = r.builder.CreateLoad(p2);
				r.builder.CreateStore(d2, p1);
				r.builder.CreateStore(d1, p2);
				return &VOID_DATA;

			} else if(s=="trim" && v.size()==0){
				/* forces alloc to be size of used memory*/
				auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));
				auto ALLOC_P = r.builder.CreateConstGEP2_32(V, 0, 2);

				auto DATA_P = r.builder.CreateConstGEP2_32(V, 0, 3);

				auto NEW_P = r.reallocate(r.builder.CreateLoad(DATA_P),AC->inner->type,LENGTH);

				r.builder.CreateStore(NEW_P,DATA_P);
				r.builder.CreateStore(LENGTH,ALLOC_P);

				return &VOID_DATA;
			} else if(s=="ensureCapacity" && v.size()==1 && v[0]->hasCastValue(&intClass)){
				/* If memory is smaller. grow to size*/
				auto NEWLEN = v[0]->evaluate(r)->castTo(r, &intClass, id)->getValue(r, id);
				auto ALLOC_P = r.builder.CreateConstGEP2_32(V, 0, 2);
				auto ALLOC = r.builder.CreateLoad(ALLOC_P);

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto REALLOC = r.CreateBlockD("realloc", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);
				r.builder.CreateCondBr(r.builder.CreateICmpSGT(NEWLEN, ALLOC), REALLOC, DONE);

				r.builder.SetInsertPoint(REALLOC);


				auto DATA_P = r.builder.CreateConstGEP2_32(V, 0, 3);
				auto NEW_P = r.reallocate(r.builder.CreateLoad(DATA_P),AC->inner->type,NEWLEN);

				r.builder.CreateStore(NEW_P,DATA_P);
				r.builder.CreateStore(NEWLEN,ALLOC_P);

				r.builder.CreateBr(DONE);

				r.builder.SetInsertPoint(DONE);
				return &VOID_DATA;
			} else if(s=="contains" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* whether it contains */

				auto TO_CHECK = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				const auto LENGTH = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(BOOLTYPE,4);
				RET_V->addIncoming(BoolClass::getValue(false), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);
				StandardLocation sl(r.builder.CreateGEP(DATA, IDX));
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_CHECK, "==")->getValue(r, id);

				r.builder.CreateCondBr(cmp, DONE, NEXT);
				RET_V->addIncoming(BoolClass::getValue(true), TO_SEARCH);

				r.builder.SetInsertPoint(NEXT);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_SEARCH);
				RET_V->addIncoming(BoolClass::getValue(false), NEXT);
				IDX->addIncoming(IDX2, NEXT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &boolClass);
			} else if(s=="remove" && v.size()==1 && v[0]->hasCastValue(AC->inner)){
				/* Idx where removed or -1 if not there*/
				auto TO_REMOVE = v[0]->evaluate(r)->castTo(r, AC->inner, id);
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_LOAD = r.CreateBlockD("to_load", FUNC);
				auto TO_SEARCH = r.CreateBlockD("to_search", FUNC);
				auto NEXT = r.CreateBlockD("next", FUNC);
				auto DEC_LENGTH = r.CreateBlockD("dec_length", FUNC);
				auto SHIFT_LEFT = r.CreateBlockD("shift_left", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);

				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_LOAD);

				r.builder.SetInsertPoint(DONE);
				auto RET_V = r.builder.CreatePHI(intClass.type,4);
				RET_V->addIncoming(getInt32(-1), STARTT);

				r.builder.SetInsertPoint(TO_LOAD);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
				r.builder.CreateBr(TO_SEARCH);

				r.builder.SetInsertPoint(TO_SEARCH);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), TO_LOAD);
				StandardLocation sl(r.builder.CreateGEP(DATA, IDX));
				LocationData ld(&sl, AC->inner);
				auto cmp = getBinop(r, id, &ld, TO_REMOVE, "==")->getValue(r, id);
				//LANG_M.getFunction(id, "print", NO_TEMPLATE, {AC->inner}).first->callFunction(r, id, {&ld}, nullptr);
				r.builder.CreateCondBr(cmp, DEC_LENGTH, NEXT);

				r.builder.SetInsertPoint(NEXT);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_SEARCH);
				RET_V->addIncoming(getInt32(-1), NEXT);
				IDX->addIncoming(IDX2, NEXT);

				r.builder.SetInsertPoint(DEC_LENGTH);
				auto lenM1 = r.builder.CreateSub(LENGTH,getInt32(1));
				r.builder.CreateStore(lenM1,LENGTH_P);
				decrementCount(r, id, &ld);//todo check
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(IDX, lenM1), DONE, SHIFT_LEFT);
				RET_V->addIncoming(IDX, DEC_LENGTH);

				r.builder.SetInsertPoint(SHIFT_LEFT);
				auto toMove = r.builder.CreatePHI(intClass.type, 2);
				toMove->addIncoming(IDX, DEC_LENGTH);
				auto fromMove = r.builder.CreateAdd(toMove, getInt32(1));
				toMove->addIncoming(fromMove, SHIFT_LEFT);
				r.builder.CreateStore(r.builder.CreateLoad(r.builder.CreateGEP(DATA, fromMove)), r.builder.CreateGEP(DATA, toMove));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(fromMove, lenM1), DONE, SHIFT_LEFT);
				RET_V->addIncoming(IDX, SHIFT_LEFT);

				r.builder.SetInsertPoint(DONE);
				return new ConstantData(RET_V, &intClass);
			} else if(s=="isEmpty" && v.size()==0){
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);
				return new ConstantData(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), &boolClass);
			} else if(s=="clear" && v.size()==0){
				/* Empties array / decrements count / does not realloc */
				auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
				const auto LENGTH = r.builder.CreateLoad(LENGTH_P);
				r.builder.CreateStore(getInt32(0),LENGTH_P);
				auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));

				auto STARTT = r.builder.GetInsertBlock();
				auto FUNC = STARTT->getParent();
				auto TO_DEC = r.CreateBlockD("to_dec", FUNC);
				auto DONE = r.CreateBlockD("done", FUNC);
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, getInt32(0)), DONE, TO_DEC);

				r.builder.SetInsertPoint(TO_DEC);
				auto IDX = r.builder.CreatePHI(intClass.type, 2);
				IDX->addIncoming(getInt32(0), STARTT);
				StandardLocation sl(r.builder.CreateGEP(DATA, IDX));
				LocationData ld(&sl, AC->inner);
				decrementCount(r, id, &ld);
				auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
				r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH, IDX2), DONE, TO_DEC);
				IDX->addIncoming(IDX2, r.builder.GetInsertBlock());

				r.builder.SetInsertPoint(DONE);
				return &VOID_DATA;
			} else break;
		}
		case CLASS_PRIORITYQUEUE:{
			if(s=="poll" && v.size()==0){

				PriorityQueueClass* AC = (PriorityQueueClass*) cc;
				static std::map<const AbstractClass*,llvm::Function*> MAP;
				llvm::Function* F;
				auto find = MAP.find(AC->inner);
				if(find==MAP.end()){
					llvm::SmallVector<llvm::Type*,1> ar(1);
					ar[0] = AC->type;
					F = r.CreateFunctionD("_opt"+AC->getName()+".poll", llvm::FunctionType::get(AC->inner->type, ar, false), LOCAL_FUNC);
					MAP.insert(std::pair<const AbstractClass*,llvm::Function*>(AC->inner,F));

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

					r.error("PriorityQueue poll() called on empty list", id);

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
