/*
 * HashMapClassP.hpp
 *
 *  Created on: Jun 29, 2014
 *      Author: Billy
 */

#ifndef HASHMAPCLASSP_HPP_
#define HASHMAPCLASSP_HPP_
#include "./HashMapClass.hpp"
#include "../../data/MapData.hpp"

HashMapClass::HashMapClass(const AbstractClass* a,const AbstractClass* b):
	AbstractClass(nullptr,str(a,b),nullptr,PRIMITIVE_LAYOUT,CLASS_HASHMAP,true,getMapType(a,b)),
	nodeType((llvm::StructType*) ( (llvm::PointerType*)  ((llvm::PointerType*)((llvm::StructType*)((llvm::PointerType*)type)->getPointerElementType())->getContainedType(3))->getPointerElementType())->getPointerElementType()),
	key(a),value(b){
	assert(a->classType!=CLASS_LAZY);
	assert(a->classType!=CLASS_REF);
	assert(b->classType!=CLASS_LAZY);
	assert(b->classType!=CLASS_REF);
	LANG_M.addFunction(PositionID(0,0,"#hashmap"),"print")->add(
		new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
		[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
		assert(args.size()==1);
		auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
		const Data* D = args[0]->evaluate(r);
		if(D->type==R_MAP){
			r.builder.CreateCall(CU, getInt32('{'));
			auto AR = (const MapData*)D;
			for(unsigned i=0; i<AR->inner.size(); i++){
				if(i>0){
					r.builder.CreateCall(CU, getInt32(','));
					r.builder.CreateCall(CU, getInt32(' '));
				}
				LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->key}).first->callFunction(r, id, {
						AR->inner[i].first->castTo(r, this->key, id)}, nullptr);
				r.builder.CreateCall(CU, getInt32(':'));
				LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->value}).first->callFunction(r, id, {
						AR->inner[i].second->castTo(r, this->value, id)}, nullptr);
			}
			r.builder.CreateCall(CU, getInt32('}'));
			return &VOID_DATA;
		}
		auto V = D->getValue(r, id);

		auto STARTT = r.builder.GetInsertBlock();
		auto FUNC = STARTT->getParent();
		auto IsNull = r.CreateBlockD("is_null", FUNC);
		auto NotNull = r.CreateBlockD("not_null", FUNC);
		auto END_NULL = r.CreateBlockD("end_n", FUNC);

		r.builder.CreateCondBr(r.builder.CreateIsNotNull(V), NotNull,IsNull);
		r.builder.SetInsertPoint(IsNull);
		for(auto& a: "null"){
			r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
		}
		r.builder.CreateBr(END_NULL);
		r.builder.SetInsertPoint(NotNull);
		r.builder.CreateCall(CU, getInt32('{'));
		auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
		auto ALLOCED = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2));
		auto PON = r.builder.CreateGEP(DATA, r.builder.CreateSub(ALLOCED, getInt32(1)));

		auto LOOP = r.CreateBlockD("loop", FUNC);
		auto NULL_B = r.CreateBlockD("null_b", FUNC);
		auto NNULL_B = r.CreateBlockD("nnull_b", FUNC);

		auto COMMA_B = r.CreateBlockD("comma", FUNC);
		auto LOOPDEC_B = r.CreateBlockD("loop_dec", FUNC);
		auto END_B = r.CreateBlockD("end", FUNC);
		r.builder.CreateBr(LOOP);

		r.builder.SetInsertPoint(LOOP);
		auto P_IDX = r.builder.CreatePHI(PON->getType(), 2);
		P_IDX->addIncoming(PON, NotNull);
		auto LOOP_HAD = r.builder.CreatePHI(BOOLTYPE, 2);
		LOOP_HAD->addIncoming(BoolClass::getValue(false), NotNull);
		auto LL = r.builder.CreateLoad(PON);


		r.builder.CreateCondBr(r.builder.CreateIsNull(LL), NULL_B,NNULL_B);

		r.builder.SetInsertPoint(NULL_B);
		auto NULL_HAD = r.builder.CreatePHI(BOOLTYPE, 2);
		NULL_HAD->addIncoming(LOOP_HAD, LOOP);
		LOOP_HAD->addIncoming(NULL_HAD, NULL_B);
		auto P_IDXM1 = r.builder.CreateConstGEP1_32(P_IDX,-1);//TODO VERIFY THAT THIS DOES DO -1
		r.builder.CreateCondBr(r.builder.CreateICmpEQ(DATA,P_IDX),END_B,LOOP);
		P_IDX->addIncoming(P_IDXM1, NULL_B);

		r.builder.SetInsertPoint(NNULL_B);
		auto LINKED = r.builder.CreatePHI(llvm::PointerType::getUnqual(this->nodeType),2);
		LINKED->addIncoming(LL, LOOP);
		auto HAD_ELEM = r.builder.CreatePHI(BOOLTYPE,2);
		HAD_ELEM->addIncoming(LOOP_HAD, LOOP);

		ConstantData KEY(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 1)), this->key);
		ConstantData VALUE(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 2)), this->value);
		LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->key}).first->callFunction(r, id, {&KEY}, nullptr);
		r.builder.CreateCall(CU, getInt32(':'));
		LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->value}).first->callFunction(r, id, {&VALUE}, nullptr);
		r.builder.CreateCondBr(HAD_ELEM, COMMA_B, LOOPDEC_B);

		r.builder.SetInsertPoint(COMMA_B);
		r.builder.CreateCall(CU, getInt32(','));
		r.builder.CreateCall(CU, getInt32(' '));
		r.builder.CreateBr(LOOPDEC_B);

		r.builder.SetInsertPoint(LOOPDEC_B);
		auto NEX=r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 0));
		r.builder.CreateCondBr(r.builder.CreateIsNull(NEX),NULL_B, NNULL_B);
		NULL_HAD->addIncoming(BoolClass::getValue(true), LOOPDEC_B);
		LINKED->addIncoming(NEX, LOOPDEC_B);
		HAD_ELEM->addIncoming(BoolClass::getValue(true), LOOPDEC_B);


		r.builder.SetInsertPoint(END_B);
		r.builder.CreateCall(CU, getInt32('}'));
		r.builder.CreateBr(END_NULL);
		r.builder.SetInsertPoint(END_NULL);
		return &VOID_DATA;
	}), PositionID(0,0,"#hashmap"));

	LANG_M.addFunction(PositionID(0,0,"#hashmap"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			const Data* D = args[0]->evaluate(r);
			if(D->type==R_MAP){
				r.builder.CreateCall(CU, getInt32('{'));
				auto AR = (const MapData*)D;
				for(unsigned i=0; i<AR->inner.size(); i++){
					if(i>0){
						r.builder.CreateCall(CU, getInt32(','));
						r.builder.CreateCall(CU, getInt32(' '));
					}
					LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->key}).first->callFunction(r, id, {
							AR->inner[i].first->castTo(r, this->key, id)}, nullptr);
					r.builder.CreateCall(CU, getInt32(':'));
					LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->value}).first->callFunction(r, id, {
							AR->inner[i].second->castTo(r, this->value, id)}, nullptr);
				}
				r.builder.CreateCall(CU, getInt32('}'));
				r.builder.CreateCall(CU, getInt32('\n'));
				return &VOID_DATA;
			}
			auto V = D->getValue(r, id);

			auto STARTT = r.builder.GetInsertBlock();
			auto FUNC = STARTT->getParent();
			auto IsNull = r.CreateBlockD("is_null", FUNC);
			auto NotNull = r.CreateBlockD("not_null", FUNC);
			auto END_NULL = r.CreateBlockD("end_n", FUNC);

			r.builder.CreateCondBr(r.builder.CreateIsNotNull(V), NotNull,IsNull);
			r.builder.SetInsertPoint(IsNull);
			for(auto& a: "null"){
				r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
			}
			r.builder.CreateCall(CU,getInt32('\n'));
			r.builder.CreateBr(END_NULL);
			r.builder.SetInsertPoint(NotNull);
			r.builder.CreateCall(CU, getInt32('{'));
			auto DATA = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
			auto ALLOCED = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2));
			auto PON = r.builder.CreateGEP(DATA, r.builder.CreateSub(ALLOCED, getInt32(1)));

			auto LOOP = r.CreateBlockD("loop", FUNC);
			auto NULL_B = r.CreateBlockD("null_b", FUNC);
			auto NNULL_B = r.CreateBlockD("nnull_b", FUNC);

			auto COMMA_B = r.CreateBlockD("comma", FUNC);
			auto LOOPDEC_B = r.CreateBlockD("loop_dec", FUNC);
			auto END_B = r.CreateBlockD("end", FUNC);

			r.println("not_null");

			r.builder.CreateBr(LOOP);

			r.builder.SetInsertPoint(LOOP);
			auto P_IDX = r.builder.CreatePHI(PON->getType(), 2);
			P_IDX->addIncoming(PON, NotNull);
			auto LOOP_HAD = r.builder.CreatePHI(BOOLTYPE, 2);
			LOOP_HAD->addIncoming(BoolClass::getValue(false), NotNull);
			auto LL = r.builder.CreateLoad(PON);

			r.println("loop");

			r.builder.CreateCondBr(r.builder.CreateIsNull(LL), NULL_B,NNULL_B);

			r.builder.SetInsertPoint(NULL_B);
			auto NULL_HAD = r.builder.CreatePHI(BOOLTYPE, 2);
			NULL_HAD->addIncoming(LOOP_HAD, LOOP);
			LOOP_HAD->addIncoming(NULL_HAD, NULL_B);
			auto P_IDXM1 = r.builder.CreateConstGEP1_32(P_IDX,-1);//TODO VERIFY THAT THIS DOES DO -1
			r.builder.CreateCondBr(r.builder.CreateICmpEQ(DATA,P_IDX),END_B,LOOP);
			P_IDX->addIncoming(P_IDXM1, NULL_B);

			r.println("null_b");

			r.builder.SetInsertPoint(NNULL_B);
			auto LINKED = r.builder.CreatePHI(llvm::PointerType::getUnqual(this->nodeType),2);
			LINKED->addIncoming(LL, LOOP);
			auto HAD_ELEM = r.builder.CreatePHI(BOOLTYPE,2);
			HAD_ELEM->addIncoming(LOOP_HAD, LOOP);

			r.println("nnull_b");

			ConstantData KEY(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 1)), this->key);
			ConstantData VALUE(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 2)), this->value);
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->key}).first->callFunction(r, id, {&KEY}, nullptr);
			r.builder.CreateCall(CU, getInt32(':'));
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->value}).first->callFunction(r, id, {&VALUE}, nullptr);
			r.builder.CreateCondBr(HAD_ELEM, COMMA_B, LOOPDEC_B);

			r.builder.SetInsertPoint(COMMA_B);
			r.builder.CreateCall(CU, getInt32(','));
			r.builder.CreateCall(CU, getInt32(' '));

			r.println("comma_b");

			r.builder.CreateBr(LOOPDEC_B);

			r.builder.SetInsertPoint(LOOPDEC_B);

			r.println("loopdec_b");

			auto NEX=r.builder.CreateLoad(r.builder.CreateConstGEP2_32(LINKED, 0, 0));
			r.builder.CreateCondBr(r.builder.CreateIsNull(NEX),NULL_B, NNULL_B);
			NULL_HAD->addIncoming(BoolClass::getValue(true), LOOPDEC_B);
			LINKED->addIncoming(NEX, LOOPDEC_B);
			HAD_ELEM->addIncoming(BoolClass::getValue(true), LOOPDEC_B);


			r.builder.SetInsertPoint(END_B);
			r.builder.CreateCall(CU, getInt32('}'));
			r.builder.CreateBr(END_NULL);
			r.builder.SetInsertPoint(END_NULL);
			r.builder.CreateCall(CU, getInt32('\n'));
			return &VOID_DATA;
		}), PositionID(0,0,"#hashmap"));
	///register methods such as print / tostring / tofile / etc
}


#endif /* HASHMAPCLASSP_HPP_ */
