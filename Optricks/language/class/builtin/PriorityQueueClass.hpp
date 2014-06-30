/*
 * PriorityQueueClass.hpp
 *
 *  Created on: Jun 27, 2014
 *      Author: Billy
 */

#ifndef PRIORITYQUEUECLASS_HPP_
#define PRIORITYQUEUECLASS_HPP_

#include "../AbstractClass.hpp"
#include "./IntClass.hpp"
class PriorityQueueClass: public AbstractClass{
public:
	static inline String str(const AbstractClass* const d){
		assert(d);
		return "PriorityQueue{"+d->getName()+"}";
	}
	static inline llvm::Type* getPriorityQueueType(const AbstractClass* const d){
		assert(d);
		llvm::SmallVector<llvm::Type*,4> ar(4);
		ar[0] = /* Counts (for garbage collection) */ intClass.type;
		ar[1] = /* Length of array */ intClass.type;
		ar[2] = /* Amount of memory allocated */ intClass.type;
		ar[3] = /* Actual data */ llvm::PointerType::getUnqual(d->type);
		return llvm::PointerType::getUnqual(llvm::StructType::get(llvm::getGlobalContext(),ar,false));
	}
	const AbstractClass* inner;
protected:
	PriorityQueueClass(const AbstractClass* a):
		AbstractClass(nullptr,str(a),nullptr,PRIMITIVE_LAYOUT,CLASS_PRIORITYQUEUE,true,getPriorityQueueType(a)),inner(a){
		assert(a->classType!=CLASS_LAZY);
		assert(a->classType!=CLASS_REF);
		assert(inner);

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
public:
	inline bool hasCast(const AbstractClass* const toCast) const{
		switch(toCast->classType){
		case CLASS_PRIORITYQUEUE: {
			auto tc = (const PriorityQueueClass*)toCast;
			return inner->hasCast(tc->inner);
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}

	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override{
		if(s=="carr") return &c_pointerClass;
		else if(s=="alloced") return &intClass;
		else if(s=="length") return &intClass;
		else{
			illegalLocal(id,s);
			exit(1);
		}
	}
	bool hasLocalData(String s) const override final{
		return s=="length" || s=="carr" || s=="alloced";
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override{
		//TODO reference count carr / make into int[len]&
		if(s=="carr"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(
					r.builder.CreatePointerCast(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 3)),C_POINTERTYPE),
					&c_pointerClass);
		} else if(s=="alloced"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 2)), &intClass);
		} else if(s=="length"){
			llvm::Value* V = instance->getValue(r,id);
			return new ConstantData(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(V, 0, 1)), &intClass);
		} else {
			illegalLocal(id,s);
			exit(1);
		}
	}
	inline bool noopCast(const AbstractClass* const toCast) const override{
		switch(toCast->classType){
		case CLASS_PRIORITYQUEUE: {
			auto tc = (PriorityQueueClass*)toCast;
			return inner->noopCast(tc->inner);
		}
		case CLASS_VOID: return true;
		default:
			return false;
		}
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const{
		if(toCast==this) return valueToCast;
		if(toCast->classType!=CLASS_PRIORITYQUEUE){
			id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		auto AR = (const PriorityQueueClass*)toCast;
		if(!inner->hasCast(AR->inner)){
			id.error("Cannot cast type '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		if(inner->noopCast(AR->inner)){
			return r.builder.CreatePointerCast(valueToCast, type);
		}
		id.compilerError("Casting priority queue types has not been implemented "+toCast->getName());
		exit(1);
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		auto fa = (PriorityQueueClass*)a;
		auto fb = (PriorityQueueClass*)b;
		return inner->compare(fa->inner, fb->inner);
	}
	static PriorityQueueClass* get(const AbstractClass* args) {
		assert(args);
		static std::map<const AbstractClass*,PriorityQueueClass*> mp;
		auto tmp = mp.find(args);
		if(tmp==mp.end()){
			return mp[args] = new PriorityQueueClass(args);
		} else return tmp->second;
	}
};

#endif /* PRIORITYQUEUECLASS_HPP_ */
