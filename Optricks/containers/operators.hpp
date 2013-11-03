/*
 * operators.hpp
 *
 *  Created on: Aug 27, 2013
 *      Author: wmoses
 */

#ifndef OPERATORS_HPP_
#define OPERATORS_HPP_

#include "operators.hpp"
#include "types.hpp"
#include "ClassProto.hpp"
bool isStartName(int i){
	return isalpha(i) || i=='_' || i=='$';
}

bool isStartType(int i){
	return isalpha(i) || i=='_' || i=='$' || i=='{' || i=='(' || i=='[';
}
Value* complexInverse(Value* a, RData& m){
	Value* sq = m.builder.CreateFMul(a, a);
	Value* a1 = m.builder.CreateExtractElement(a, getInt32(0));
	Value* a2 = m.builder.CreateExtractElement(a, getInt32(1));
	Value* denom = m.builder.CreateFAdd(m.builder.CreateExtractElement(sq, getInt32(0)), m.builder.CreateExtractElement(sq, getInt32(1)));
	Value* re = m.builder.CreateFDiv(a1, denom);
	Value* im = m.builder.CreateFNeg(m.builder.CreateFDiv(a2, denom));
	a = UndefValue::get(COMPLEXTYPE);
	a = m.builder.CreateInsertElement(a, re, getInt32(0));
	a = m.builder.CreateInsertElement(a, im, getInt32(1));
	return a;
}
Value* complexMultiply(Value* a, Value* b, RData& m){
	Value* sq = m.builder.CreateFMul(a, b);
	Value* re = m.builder.CreateFSub(m.builder.CreateExtractElement(sq, getInt32(0)), m.builder.CreateExtractElement(sq, getInt32(1)));
	Value* a1 = m.builder.CreateExtractElement(a, getInt32(0));
	Value* a2 = m.builder.CreateExtractElement(a, getInt32(1));
	Value* b1 = m.builder.CreateExtractElement(b, getInt32(0));
	Value* b2 = m.builder.CreateExtractElement(b, getInt32(1));
	Value* im = m.builder.CreateFAdd(m.builder.CreateFMul(a1, b2), m.builder.CreateFMul(a2, b1));
	Value* vec = UndefValue::get(COMPLEXTYPE);
	vec = m.builder.CreateInsertElement(vec, re, getInt32(0));
	vec = m.builder.CreateInsertElement(vec, im, getInt32(1));
	return vec;
}
Value* complexSquare(Value* a, RData& m){
	Value* sq = m.builder.CreateFMul(a, a);
	Value* re = m.builder.CreateFSub(m.builder.CreateExtractElement(sq, getInt32(0)), m.builder.CreateExtractElement(sq, getInt32(1)));
	Value* a1 = m.builder.CreateExtractElement(a, getInt32(0));
	Value* a2 = m.builder.CreateExtractElement(a, getInt32(1));
	Value* mul = m.builder.CreateFMul(a1, a2);
	Value* im = m.builder.CreateFAdd(mul,mul);
	Value* vec = UndefValue::get(COMPLEXTYPE);
	vec = m.builder.CreateInsertElement(vec, re, getInt32(0));
	vec = m.builder.CreateInsertElement(vec, im, getInt32(1));
	return vec;
}
//Function* strLen = NULL;
void initClassesMeta(){
	complexClass->addElement("real",doubleClass,PositionID(0,0,"<start.initClassesMeta>"));
	complexClass->addElement("imag",doubleClass,PositionID(0,0,"<start.initClassesMeta>"));
	//stringClass->addElement("_cstr",c_stringClass,PositionID(0,0,"<start.initClassesMeta>"));
	//stringClass->addElement("_size",intClass,PositionID(0,0,"<start.initClassesMeta>"));

	complexClass->preops["-"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value* a = av.getValue(m);
		if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
			ConstantFP* a = dyn_cast<ConstantFP>(c->getAggregateElement(0U));
			ConstantFP* b = dyn_cast<ConstantFP>(c->getAggregateElement(1U));
			APFloat a1(a->getValueAPF());
			APFloat b1(b->getValueAPF());
			a1.changeSign();
			b1.changeSign();
			std::vector<Constant*> vals = {getDouble(a1),getDouble(b1)};
			return DATA::getConstant(ConstantVector::get(vals),complexClass);
		}
		return DATA::getConstant(m.builder.CreateFNeg(a,"negtmp"),complexClass);
	},complexClass);
	complexClass->preops["+"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return av.toValue(m);
	},complexClass);
/*	functionClass->addCast(c_pointerClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Function* v = (Function*) av.getValue(m);
		auto op =  CastInst::getCastOpcode(v,false,C_POINTERTYPE,false);
		DATA d= DATA::getConstant(CastInst::Create(op, v, C_POINTERTYPE),c_pointerClass);
		//m.lmod->dump();
		//cerr <<
				m.builder.GetInsertBlock()->dump();
		cerr << m.builder.GetInsertBlock()->getName().str() << endl;
		//cerr <<
				m.builder.GetInsertBlock()->getParent()->dump();
				FUNCTIONTYPE->dump();
				cerr << endl << flush;
						//<< endl << flush;
		return d;
	},c_pointerClass
	);*/
	intClass->addCast(doubleClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m);
		if(ConstantInt* c = dyn_cast<ConstantInt>(a)){
			APInt a = c->getValue();
			return DATA::getConstant(getDouble(a.signedRoundToDouble()),doubleClass);
		}
		return DATA::getConstant(m.builder.CreateSIToFP(a,DOUBLETYPE),doubleClass);
	},doubleClass);
	//TODO allow printf
	intClass->addCast(complexClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m);
		if(ConstantInt* c = dyn_cast<ConstantInt>(a)){
			APInt a = c->getValue();
			double data[2] = {a.signedRoundToDouble(), 0};
			return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
		}
		Value* v = m.builder.CreateSIToFP(a,DOUBLETYPE);
		double data[2] = {0, 0} ;
		Value* vec = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data));
		return DATA::getConstant(m.builder.CreateInsertElement(vec,v,getInt32(0)),complexClass);
	}
	,complexClass);

	doubleClass->addCast(complexClass) = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m);
		if(ConstantFP* c = dyn_cast<ConstantFP>(a)){
			APFloat a = c->getValueAPF();
			double data[2] = {a.convertToDouble(), 0};
			return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
		}
		double data[2] = {0, 0} ;
		auto vec = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data));
		return DATA::getConstant(m.builder.CreateInsertElement(vec,a,getInt32(0)),complexClass);
	}
	,complexClass);

	complexClass->addBinop("+", complexClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = dyn_cast<ConstantFP>(c->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double a2 = dyn_cast<ConstantFP>(c->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {a1+b1, a2+b2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		return DATA::getConstant(m.builder.CreateFAdd(a,b),complexClass);
	},complexClass);
	complexClass->addBinop("-", complexClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = dyn_cast<ConstantFP>(c->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double a2 = dyn_cast<ConstantFP>(c->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {a1-b1, a2-b2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		return DATA::getConstant(m.builder.CreateFSub(a,b),complexClass);
	},complexClass);

	doubleClass->addBinop("*", complexClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantFP* c = dyn_cast<ConstantFP>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = c->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {a1*b1, a1*b2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		Value* re = m.builder.CreateFMul(a, m.builder.CreateExtractElement(b, getInt32(0)));
		Value* im = m.builder.CreateFMul(a, m.builder.CreateExtractElement(b, getInt32(1)));
		Value* vec = UndefValue::get(COMPLEXTYPE);
		vec = m.builder.CreateInsertElement(vec, re, getInt32(0));
		vec = m.builder.CreateInsertElement(vec, im, getInt32(1));
		return DATA::getConstant(vec, complexClass);
	},complexClass);

	complexClass->addBinop("*", doubleClass) = new obinopNative(
			[](DATA bv, DATA av, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantFP* c = dyn_cast<ConstantFP>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = c->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {a1*b1, a1*b2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		Value* re = m.builder.CreateFMul(a, m.builder.CreateExtractElement(b, getInt32(0)));
		Value* im = m.builder.CreateFMul(a, m.builder.CreateExtractElement(b, getInt32(1)));
		Value* vec = UndefValue::get(COMPLEXTYPE);
		vec = m.builder.CreateInsertElement(vec, re, getInt32(0));
		vec = m.builder.CreateInsertElement(vec, im, getInt32(1));
		return DATA::getConstant(vec,complexClass);
	},complexClass);

	complexClass->addBinop("*", complexClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = dyn_cast<ConstantFP>(c->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double a2 = dyn_cast<ConstantFP>(c->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {a1*b1-a2*b2, a1*b2+b1*a2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		return DATA::getConstant(complexMultiply(a,b,m),complexClass);
	},complexClass);

	/**
	 * Hyper-optimization of power operator (can be used by ints/doubles)
	 */
	complexClass->addBinop("**", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantInt* d = dyn_cast<ConstantInt>(b)){
			auto b1 = d->getValue().getSExtValue();
			if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
				double a1 = dyn_cast<ConstantFP>(c->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double a2 = dyn_cast<ConstantFP>(c->getAggregateElement(1U))->getValueAPF().convertToDouble();
				if(b1<0){
					double denom = a1*a1+a2*a2;
					a1 /=  denom;
					a2 /= -denom;
					b1=-b1;
				}
				if(b1==0){
					a1 = a2 = 0.;
				} else{
					double result1 = 1;
					double result2 = 0;
					while(true){
						if(b1%2==1){
							double tmp1 = result1*a1-result2*a2;
							double tmp2 = result1*a2+result2*a1;
							result1 = tmp1;
							result2 = tmp2;
							if(b1==1) break;
						}
						double tmp1 = a1*a1-a2*a2;
						double tmp2 = 2*a1*a2;
						a1 = tmp1;
						a2 = tmp2;
						b1 /= 2;
					}
					a1 = result1;
					a2 = result2;
				}
				double data[2] = {a1, a2};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
			if(b1==0){
				double data[2] = {1,0};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
			if(b1<0){
				a = complexInverse(a, m);
				b1=-b1;
			}
			Value* result = NULL;
			while(true){
				if(b1%2==1){
					if(result==NULL) result = a;
					else result = complexMultiply(result, a, m);
					if(b1==1) break;
				}
				a = complexSquare(a,m);
				b1 /= 2;
			}
			return DATA::getConstant(result,complexClass);
		}

		Function *TheFunction = m.builder.GetInsertBlock()->getParent();
		BasicBlock *ThenBB = BasicBlock::Create(m.lmod->getContext(), "if_zero", TheFunction);
		BasicBlock *ElseBB = BasicBlock::Create(m.lmod->getContext(), "_nonzero", TheFunction);
		BasicBlock *MergeBB = BasicBlock::Create(m.lmod->getContext(), "all_pow", TheFunction);
		m.builder.CreateCondBr(m.builder.CreateICmpEQ(b,getInt(0)), ThenBB, ElseBB);
		m.builder.SetInsertPoint(ThenBB);
		double temp1[2] = {1,0};
		Value* ifZero = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(temp1));
		m.builder.CreateBr(MergeBB);
			//INCOMING ifZero MergeBB
		m.builder.SetInsertPoint(ElseBB);
		BasicBlock *LessBB = BasicBlock::Create(m.lmod->getContext(), "_lesszero", TheFunction);
		BasicBlock *PreLoopBB = BasicBlock::Create(m.lmod->getContext(), "_preLoop", TheFunction);
		BasicBlock *GreatBB = BasicBlock::Create(m.lmod->getContext(), "_greatzero", TheFunction);
		m.builder.CreateCondBr(m.builder.CreateICmpSLT(b, getInt(0)), LessBB, PreLoopBB);
		m.builder.SetInsertPoint(PreLoopBB);
		Value* subOne = m.builder.CreateSub(b, getInt(1));
		m.builder.CreateCondBr(m.builder.CreateICmpEQ(subOne, getInt(0)), MergeBB, GreatBB);
		m.builder.SetInsertPoint(LessBB);
		Value* inversed = complexInverse(a,m);
		Value* realExp = m.builder.CreateSub(getInt(-1), b);
		m.builder.CreateCondBr(m.builder.CreateICmpEQ(realExp,getInt(0)), MergeBB,GreatBB);
		m.builder.SetInsertPoint(GreatBB);
		PHINode* X = m.builder.CreatePHI(COMPLEXTYPE, 3, "temp");
		X->addIncoming(inversed, LessBB);
		X->addIncoming(a, PreLoopBB);
		//X->addIncoming();
		PHINode* PN = m.builder.CreatePHI(COMPLEXTYPE, 3, "result");
		PN->addIncoming(inversed, LessBB);
		PN->addIncoming(a, PreLoopBB);
		//PN->addIncoming();
		PHINode* exp = m.builder.CreatePHI(INTTYPE, 3, "pow");
		exp->addIncoming(realExp, LessBB);
		exp->addIncoming(subOne, PreLoopBB);
		//exp->addIncoming();
		BasicBlock *OddBB = BasicBlock::Create(m.lmod->getContext(), "if_odd", TheFunction);
		BasicBlock *EvenBB = BasicBlock::Create(m.lmod->getContext(), "if_even", TheFunction);
		m.builder.CreateCondBr(m.builder.CreateTrunc(exp, BOOLTYPE), OddBB, EvenBB);
		m.builder.SetInsertPoint(OddBB);
		Value* resultOdd = complexMultiply(PN, X, m);
		m.builder.CreateCondBr(m.builder.CreateICmpEQ(exp, getInt(1)), MergeBB, EvenBB);
			//INCOMING resuldOdd to MergeBB;
		m.builder.SetInsertPoint(EvenBB);
		PHINode* res = m.builder.CreatePHI(COMPLEXTYPE, 2, "res2");
		res->addIncoming(resultOdd, OddBB);
		res->addIncoming(PN, GreatBB);
		Value* nExp = m.builder.CreateUDiv(exp, getInt(2));
		Value* nX = complexSquare(X, m);
		m.builder.CreateBr(GreatBB);
			//INCOMING res to MergeBB;
		X->addIncoming(nX, EvenBB);
		PN->addIncoming(res, EvenBB);
		exp->addIncoming(nExp, EvenBB);
		TheFunction->getBasicBlockList().remove(MergeBB);
		TheFunction->getBasicBlockList().push_back(MergeBB);
		m.builder.SetInsertPoint(MergeBB);
		PHINode* comp = m.builder.CreatePHI(COMPLEXTYPE, 4, "finalExp");
		comp->addIncoming(ifZero, ThenBB);
		comp->addIncoming(resultOdd, OddBB);
		comp->addIncoming(inversed, LessBB);
		comp->addIncoming(a, PreLoopBB);
		//comp->addIncoming(res, EvenBB);
		return DATA::getConstant(comp,complexClass);
	},complexClass);

	complexClass->addBinop("/", doubleClass) = new obinopNative(
			[](DATA bv, DATA av, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantFP* c = dyn_cast<ConstantFP>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = c->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double data[2] = {b1/a1, b2/a1};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)),complexClass);
			}
		}
		Value* re = m.builder.CreateFDiv(m.builder.CreateExtractElement(b, getInt32(0)),a);
		Value* im = m.builder.CreateFDiv(m.builder.CreateExtractElement(b, getInt32(1)),a);
		Value* vec = UndefValue::get(COMPLEXTYPE);
		vec = m.builder.CreateInsertElement(vec, re, getInt32(0));
		vec = m.builder.CreateInsertElement(vec, im, getInt32(1));
		return DATA::getConstant(vec, complexClass);
	},complexClass);

	complexClass->addBinop("/", complexClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		if(ConstantVector* c = dyn_cast<ConstantVector>(a)){
			if(ConstantVector* d = dyn_cast<ConstantVector>(b)){
				double a1 = dyn_cast<ConstantFP>(c->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double a2 = dyn_cast<ConstantFP>(c->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double b1 = dyn_cast<ConstantFP>(d->getAggregateElement(0U))->getValueAPF().convertToDouble();
				double b2 = dyn_cast<ConstantFP>(d->getAggregateElement(1U))->getValueAPF().convertToDouble();
				double denom = b1*b1+b2*b2;
				double data[2] = {(a1*b1+a2*b2)/denom, (b1*a2-a1*b2)/denom};
				return DATA::getConstant(ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data)), complexClass);
			}
		}
		Value* sq = m.builder.CreateFMul(a, b);
		Value* bSquared = m.builder.CreateFMul(b, b);
		Value* denom = m.builder.CreateFAdd(m.builder.CreateExtractElement(bSquared, getInt32(0)), m.builder.CreateExtractElement(bSquared, getInt32(1)));
		Value* re = m.builder.CreateFAdd(m.builder.CreateExtractElement(sq, getInt32(0)), m.builder.CreateExtractElement(sq, getInt32(1)));
		Value* a1 = m.builder.CreateExtractElement(a, getInt32(0));
		Value* a2 = m.builder.CreateExtractElement(a, getInt32(1));
		Value* b1 = m.builder.CreateExtractElement(b, getInt32(0));
		Value* b2 = m.builder.CreateExtractElement(b, getInt32(1));
		Value* im = m.builder.CreateFSub(m.builder.CreateFMul(a2, b1), m.builder.CreateFMul(a1, b2));
		Value* vec = UndefValue::get(COMPLEXTYPE);
		vec = m.builder.CreateInsertElement(vec, m.builder.CreateFDiv(re,denom), getInt32(0));
		vec = m.builder.CreateInsertElement(vec, m.builder.CreateFDiv(im,denom), getInt32(1));
		return DATA::getConstant(vec,complexClass);
	},complexClass);

	/*
	//TODO ALLOW FOR CONSTANTS FROM HERE DOWN (and constant / fast exponentiation)
	stringClass->addBinop("[]",intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{

		Value *a = av.getValue(m), *b = bv.getValue(m);
		Value* val = m.builder.CreateConstGEP2_32(m.builder.CreateLoad(a),0,stringClass->getDataClassIndex("_cstr",id));
		val->dump();
		val->getType()->dump();
		cerr << endl << flush;
		std::vector<Value*> v = {m.builder.CreateTruncOrBitCast(b,INT32TYPE)};
		auto t = m.builder.CreateInBoundsGEP(val,v,"tmpind");
		return DATA::getConstant(m.builder.CreateLoad(t),charClass);
		//				return m.builder.CreateAnd(a,b,"andtmp");
	},charClass);*/

	c_stringClass->addBinop("[]",intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		Value *a = av.getValue(m), *b = bv.getValue(m);
		std::vector<Value*> z = {m.builder.CreateTruncOrBitCast(b,INT32TYPE)};
		Value* t = m.builder.CreateInBoundsGEP(a,z,"tmpind");
		return DATA::getConstant(m.builder.CreateLoad(t),charClass);
	},charClass);


	///////******************************* Boolean ********************************////////
	boolClass->addBinop("&&",boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateAnd(av.getValue(m),bv.getValue(m),"andtmp"),boolClass);
		//still short circuits
	},boolClass);

	boolClass->addBinop("||",boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateOr(av.getValue(m),bv.getValue(m),"ortmp"),boolClass);
		//still short circuits
	},boolClass);

	boolClass->addBinop("&", boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateAnd(av.getValue(m),bv.getValue(m),"andtmp"),boolClass);
	},boolClass);

	boolClass->addBinop("|", boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateOr(av.getValue(m),bv.getValue(m),"ortmp"),boolClass);
	},boolClass);

	boolClass->addBinop("^", boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateXor(av.getValue(m),bv.getValue(m),"xortmp"),boolClass);
	},boolClass);

	boolClass->addBinop("!=", boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpNE(av.getValue(m),bv.getValue(m),"andtmp"),boolClass);
	},boolClass);

	boolClass->addBinop("==", boolClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpEQ(av.getValue(m),bv.getValue(m),"andtmp"),boolClass);
	},boolClass);

	boolClass->preops["!"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateNot(av.getValue(m),"nottmp"),boolClass);
	},boolClass);

	///////******************************* Double/Double ******************************////////
	doubleClass->addBinop("+", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFAdd(av.getValue(m),bv.getValue(m),"addtmp"),doubleClass);
	},doubleClass);

	doubleClass->addBinop("-", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFSub(av.getValue(m),bv.getValue(m),"subtmp"),doubleClass);
	},doubleClass);

	doubleClass->addBinop("*", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFMul(av.getValue(m),bv.getValue(m),"multmp"),doubleClass);
	},doubleClass);

	doubleClass->addBinop("%", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFRem(av.getValue(m),bv.getValue(m),"modtmp"),doubleClass);
	},doubleClass);

	doubleClass->addBinop("<", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpOLT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop(">", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpOGT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop("<=", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpOLE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop(">=", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpOGE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop("==", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpOEQ(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop("!=", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFCmpONE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	doubleClass->addBinop("/", doubleClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFDiv(av.getValue(m),bv.getValue(m),"divtmp"),doubleClass);
	},doubleClass);

	doubleClass->preops["-"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateFNeg(av.getValue(m),"negtmp"),doubleClass);
	},doubleClass);

	doubleClass->preops["+"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return av.toValue(m);
	},doubleClass);


	///////******************************* INT ********************************////////
	intClass->addBinop("&", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateAnd(av.getValue(m),bv.getValue(m),"andtmp"),intClass);
	},intClass);

	intClass->addBinop("|", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateOr(av.getValue(m),bv.getValue(m),"ortmp"),intClass);
	},intClass);

	intClass->addBinop("^", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateXor(av.getValue(m),bv.getValue(m),"xortmp"),intClass);
	},intClass);

	intClass->addBinop("+", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateAdd(av.getValue(m),bv.getValue(m),"addtmp"),intClass);
	},intClass);

	intClass->addBinop("-", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateSub(av.getValue(m),bv.getValue(m),"subtmp"),intClass);
	},intClass);

	intClass->addBinop("*", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateMul(av.getValue(m),bv.getValue(m),"multmp"),intClass);
	},intClass);

	intClass->addBinop("%", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateSRem(av.getValue(m),bv.getValue(m),"modtmp"),intClass);
	},intClass);

	intClass->addBinop("<", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSLT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop(">", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSGT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop("<=", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSLE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop(">=", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSGE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop("==", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpEQ(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop("!=", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpNE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	intClass->addBinop("/", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateSDiv(av.getValue(m),bv.getValue(m),"divtmp"),intClass);
	},intClass);

	intClass->addBinop("<<", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateShl(av.getValue(m),bv.getValue(m)),intClass);
	},intClass);

	intClass->addBinop(">>", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateAShr(av.getValue(m),bv.getValue(m)),intClass);
	},intClass);

	intClass->addBinop(">>>", intClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateLShr(av.getValue(m),bv.getValue(m)),intClass);
	},intClass);

	intClass->preops["-"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateNeg(av.getValue(m),"negtmp"),intClass);
	},intClass);

	intClass->preops["+"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return av.toValue(m);
	},intClass);

	intClass->preops["--"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		if(av.getType()!=R_LOC) id.error("Need variable to use '--' pre-operator on integer");
		Value* tmp = av.getValue(m);
		av.setValue(m, m.builder.CreateSub(tmp, getInt(1)));
		return DATA::getConstant(tmp, intClass);
	},intClass);
	intClass->preops["++"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		if(av.getType()!=R_LOC) id.error("Need variable to use '++' pre-operator on integer");
		Value* tmp = av.getValue(m);
		av.setValue(m, m.builder.CreateAdd(tmp, getInt(1)));
		return DATA::getConstant(tmp, intClass);
	},intClass);
	intClass->postops["--"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		if(av.getType()!=R_LOC) id.error("Need variable to use '--' post-operator on integer");
		av.setValue(m, m.builder.CreateSub(av.getValue(m), getInt(1)));
		return av;
	},intClass);
	intClass->postops["++"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		if(av.getType()!=R_LOC) id.error("Need variable to use '++' post-operator on integer");
		av.setValue(m, m.builder.CreateAdd(av.getValue(m), getInt(1)));
		return av;
	},intClass);

	intClass->preops["~"] = new ouopNative(
			[](DATA av, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateNot(av.getValue(m),"negtmp"),intClass);
	},intClass);

	///////******************************* BYTE ********************************////////
		byteClass->addBinop("&", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateAnd(av.getValue(m),bv.getValue(m),"andtmp"),intClass);
		},byteClass);

		byteClass->addBinop("|", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateOr(av.getValue(m),bv.getValue(m),"ortmp"),intClass);
		},byteClass);

		byteClass->addBinop("^", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateXor(av.getValue(m),bv.getValue(m),"xortmp"),intClass);
		},intClass);

		byteClass->addBinop("+", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateAdd(av.getValue(m),bv.getValue(m),"addtmp"),intClass);
		},byteClass);

		byteClass->addBinop("-", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateSub(av.getValue(m),bv.getValue(m),"subtmp"),intClass);
		},byteClass);

		byteClass->addBinop("*", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateMul(av.getValue(m),bv.getValue(m),"multmp"),intClass);
		},byteClass);

		byteClass->addBinop("%", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateSRem(av.getValue(m),bv.getValue(m),"modtmp"),intClass);
		},byteClass);

		byteClass->addBinop("<", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpULT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},byteClass);

		byteClass->addBinop(">", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpUGT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},boolClass);

		byteClass->addBinop("<=", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpULE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},boolClass);

		byteClass->addBinop(">=", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpUGE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},boolClass);

		byteClass->addBinop("==", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpEQ(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},boolClass);

		byteClass->addBinop("!=", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateICmpNE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
		},boolClass);

		byteClass->addBinop("/", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateUDiv(av.getValue(m),bv.getValue(m),"divtmp"),intClass);
		},byteClass);

		byteClass->addBinop("<<", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateShl(av.getValue(m),bv.getValue(m)),intClass);
		},intClass);

		byteClass->addBinop(">>", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateLShr(av.getValue(m),bv.getValue(m)),intClass);
//			return DATA::getConstant(m.builder.CreateAShr(av.getValue(m),bv.getValue(m)),intClass);
		},byteClass);

		byteClass->addBinop(">>>", byteClass) = new obinopNative(
				[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateLShr(av.getValue(m),bv.getValue(m)),intClass);
		},byteClass);

		byteClass->preops["+"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			return av.toValue(m);
		},byteClass);

		byteClass->preops["--"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			if(av.getType()!=R_LOC) id.error("Need variable to use '--' pre-operator on byte");
			Value* tmp = av.getValue(m);
			av.setValue(m, m.builder.CreateSub(tmp, getByte(1)));
			return DATA::getConstant(tmp, intClass);
		},intClass);
		byteClass->preops["++"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			if(av.getType()!=R_LOC) id.error("Need variable to use '++' pre-operator on byte");
			Value* tmp = av.getValue(m);
			av.setValue(m, m.builder.CreateAdd(tmp, getByte(1)));
			return DATA::getConstant(tmp, intClass);
		},byteClass);
		byteClass->postops["--"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			if(av.getType()!=R_LOC) id.error("Need variable to use '--' post-operator on byte");
			av.setValue(m, m.builder.CreateSub(av.getValue(m), getByte(1)));
			return av;
		},byteClass);
		byteClass->postops["++"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			if(av.getType()!=R_LOC) id.error("Need variable to use '++' post-operator on byte");
			av.setValue(m, m.builder.CreateAdd(av.getValue(m), getByte(1)));
			return av;
		},byteClass);

		byteClass->preops["~"] = new ouopNative(
				[](DATA av, RData& m, PositionID id) -> DATA{
			return DATA::getConstant(m.builder.CreateNot(av.getValue(m),"negtmp"),intClass);
		},byteClass);
	//############################ CHAR


	charClass->addBinop("<", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSLT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	charClass->addBinop(">", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSGT(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	charClass->addBinop("<=", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSLE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	charClass->addBinop(">=", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpSGE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	charClass->addBinop("==", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpEQ(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);

	charClass->addBinop("!=", charClass) = new obinopNative(
			[](DATA av, DATA bv, RData& m, PositionID id) -> DATA{
		return DATA::getConstant(m.builder.CreateICmpNE(av.getValue(m),bv.getValue(m),"cmptmp"),boolClass);
	},boolClass);
	/*
	LANG_M->addPointer("class",classClass,0);
	LANG_M->addPointer("object",objectClass,0);
	LANG_M->addPointer("bool",boolClass,0);
	LANG_M->addPointer("array",arrayClass,0);
	LANG_M->addPointer("function",functionClass,0);
	LANG_M->addPointer("int",intClass,0);
	LANG_M->addPointer("double",doubleClass,0);
	LANG_M->addPointer("string",stringClass,0);
	LANG_M->addPointer("slice",sliceClass,0);*/
}



#endif /* OPERATORS_HPP_ */
