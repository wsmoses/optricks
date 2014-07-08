/*
 * ArrayClassP.hpp
 *
 *  Created on: Mar 8, 2014
 *      Author: Billy
 */

#ifndef ARRAYCLASSP_HPP_
#define ARRAYCLASSP_HPP_

#include "./ArrayClass.hpp"
#include "../../data/ArrayData.hpp"
ArrayClass::ArrayClass(const AbstractClass* a):
		AbstractClass(nullptr,str(a),nullptr,PRIMITIVE_LAYOUT,CLASS_ARRAY,true,getArrayType(a)),inner(a){
		if(a){
			assert(a->classType!=CLASS_LAZY);
			assert(a->classType!=CLASS_REF);
		}
		LANG_M.addFunction(PositionID(0,0,"#array"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			const Data* D = args[0]->evaluate(r);
			if(D->type==R_ARRAY){
				r.builder.CreateCall(CU, getInt32('['));
				auto AR = (const ArrayData*)D;
				for(unsigned i=0; i<AR->inner.size(); i++){
					if(i>0){
						r.builder.CreateCall(CU, getInt32(','));
						r.builder.CreateCall(CU, getInt32(' '));
					}
					LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->inner}).first->callFunction(r, id, {
							(this->inner)?(AR->inner[i]->castTo(r, this->inner, id)):AR->inner[i]}, nullptr);
				}
				r.builder.CreateCall(CU, getInt32(']'));
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
			r.builder.CreateCall(CU, getInt32('['));
			auto DATA_P = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
			auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
			auto LENGTH = r.builder.CreateLoad(LENGTH_P);

			auto ONE = r.CreateBlockD("one", FUNC);
			auto MANY = r.CreateBlockD("many", FUNC);
			auto END = r.CreateBlockD("end", FUNC);

			r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), END,ONE);

			r.builder.SetInsertPoint(ONE);
			auto IDX = r.builder.CreatePHI(intClass.type,2);
			auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
			IDX->addIncoming(getInt32(0),NotNull);
			IDX->addIncoming(IDX2, MANY);
			ConstantData cd(r.builder.CreateLoad(r.builder.CreateGEP(DATA_P,IDX)), this->inner);
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->inner}).first->callFunction(r, id, {&cd}, nullptr);
			r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH,IDX2), END, MANY);

			r.builder.SetInsertPoint(MANY);
			r.builder.CreateCall(CU, getInt32(','));
			r.builder.CreateCall(CU, getInt32(' '));
			r.builder.CreateBr(ONE);

			r.builder.SetInsertPoint(END);
			r.builder.CreateCall(CU, getInt32(']'));
			r.builder.CreateBr(END_NULL);
			r.builder.SetInsertPoint(END_NULL);
			return &VOID_DATA;
		}), PositionID(0,0,"#complex"));

		LANG_M.addFunction(PositionID(0,0,"#array"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
			assert(args.size()==1);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});

			const Data* D = args[0]->evaluate(r);
			if(D->type==R_ARRAY){
				r.builder.CreateCall(CU, getInt32('['));
				auto AR = (const ArrayData*)D;
				for(unsigned i=0; i<AR->inner.size(); i++){
					if(i>0){
						r.builder.CreateCall(CU, getInt32(','));
						r.builder.CreateCall(CU, getInt32(' '));
					}
					LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->inner}).first->callFunction(r, id, {
							(this->inner)?(AR->inner[i]->castTo(r, this->inner, id)):AR->inner[i]}, nullptr);
				}
				r.builder.CreateCall(CU, getInt32(']'));
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
			r.builder.CreateBr(END_NULL);
			r.builder.SetInsertPoint(NotNull);
			r.builder.CreateCall(CU, getInt32('['));
			auto DATA_P = r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3));
			auto LENGTH_P = r.builder.CreateConstGEP2_32(V, 0, 1);
			auto LENGTH = r.builder.CreateLoad(LENGTH_P);

			auto ONE = r.CreateBlockD("one", FUNC);
			auto MANY = r.CreateBlockD("many", FUNC);
			auto END = r.CreateBlockD("end", FUNC);

			r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH,getInt32(0)), END,ONE);

			r.builder.SetInsertPoint(ONE);
			auto IDX = r.builder.CreatePHI(intClass.type,2);
			auto IDX2 = r.builder.CreateAdd(IDX, getInt32(1));
			IDX->addIncoming(getInt32(0),NotNull);
			IDX->addIncoming(IDX2, MANY);
			ConstantData cd(r.builder.CreateLoad(r.builder.CreateGEP(DATA_P,IDX)), this->inner);
			LANG_M.getFunction(id, "print", NO_TEMPLATE, {this->inner}).first->callFunction(r, id, {&cd}, nullptr);
			r.builder.CreateCondBr(r.builder.CreateICmpEQ(LENGTH,IDX2), END, MANY);

			r.builder.SetInsertPoint(MANY);
			r.builder.CreateCall(CU, getInt32(','));
			r.builder.CreateCall(CU, getInt32(' '));
			r.builder.CreateBr(ONE);

			r.builder.SetInsertPoint(END);
			r.builder.CreateCall(CU, getInt32(']'));
			r.builder.CreateBr(END_NULL);
			r.builder.SetInsertPoint(END_NULL);
			r.builder.CreateCall(CU, getInt32('\n'));
			return &VOID_DATA;
		}), PositionID(0,0,"#complex"));
		///register methods such as print / tostring / tofile / etc
	}
llvm::Value* ArrayClass::castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
	if(toCast==this) return valueToCast;
	if(toCast->classType!=CLASS_ARRAY){
		id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
		exit(1);
	}
	if(!inner){
		return toCast->callFunction(r, id, {}, nullptr)->getValue(r,id);
	}
	auto AR = (const ArrayClass*)toCast;
	if(!AR->inner || !inner->hasCast(AR->inner)){
		id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
		exit(1);
	}
	if(inner->noopCast(AR->inner)){
		assert(AR->inner->type->isPointerTy());
		return r.pointerCast(valueToCast, (llvm::PointerType*) AR->inner->type);
	}
	cerr << this << " " << toCast << endl << flush;
	id.compilerError("Casting array types has not been implemented "+toCast->getName());
	exit(1);
}


#endif /* ARRAYCLASSP_HPP_ */
