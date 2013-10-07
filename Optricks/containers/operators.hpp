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
//Function* strLen = NULL;
void initClassesMeta(){
	complexClass->addElement("real",doubleClass,PositionID());
	complexClass->addElement("imag",doubleClass,PositionID());
	stringClass->addElement("_cstr",c_stringClass,PositionID());
	stringClass->addElement("_size",intClass,PositionID());

	complexClass->preops["-"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return m.builder.CreateFNeg(a,"negtmp");
	},complexClass);
	complexClass->preops["+"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return a;
	},complexClass);

	intClass->addCast(doubleClass) = new ouopNative(
			[](DATA a, RData& m) -> DATA{
				return m.builder.CreateSIToFP(a,DOUBLETYPE);
	},doubleClass);

	intClass->addCast(complexClass) = new ouopNative(
			[](DATA a, RData& m) -> DATA{
			auto v = m.builder.CreateSIToFP(a,DOUBLETYPE);
			double data[2] = {0, 0} ;
			auto vec = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data));
			return m.builder.CreateInsertElement(vec,v,getInt32(0));
	}
	,complexClass);

	doubleClass->addCast(complexClass) = new ouopNative(
			[](DATA a, RData& m) -> DATA{
		double data[2] = {0, 0} ;
		auto vec = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(data));
		return m.builder.CreateInsertElement(vec,a,getInt32(0));
	}
	,complexClass);

	complexClass->addBinop("+", complexClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateFAdd(a,b);
	},complexClass);
	complexClass->addBinop("-", complexClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateFSub(a,b);
	},complexClass);
	complexClass->addBinop("[]",intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateExtractElement(a,m.builder.CreateTruncOrBitCast(b,INT32TYPE));
		},doubleClass);
	///////******************************* String ********************************////////
	stringClass->addBinop("[]",intClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{

//		return m.builder.CreateExtractElement(a,b);
				std::vector<Value*> z = {getInt32(0),m.builder.CreateTruncOrBitCast(b,INT32TYPE)};
				auto t = m.builder.CreateGEP(a,z,"tmpind");
				return m.builder.CreateLoad(t);
//				return m.builder.CreateAnd(a,b,"andtmp");
	},charClass);
	c_stringClass->addBinop("[]",intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{

	//		return m.builder.CreateExtractElement(a,b);
					std::vector<Value*> z = {m.builder.CreateTruncOrBitCast(b,INT32TYPE)};
					auto t = m.builder.CreateGEP(a,z,"tmpind");
					return m.builder.CreateLoad(t);
	//				return m.builder.CreateAnd(a,b,"andtmp");
		},charClass);


	///////******************************* Boolean ********************************////////
	/* //TODO short-circuit
	boolClass->binops["&&"][boolClass] = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->binops["||"][boolClass] = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);
	*/
	boolClass->addBinop("&", boolClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->addBinop("|", boolClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);

	boolClass->addBinop("^", boolClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateXor(a,b,"xortmp");
	},boolClass);

	boolClass->addBinop("!=", boolClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateICmpNE(a,b,"andtmp");
	},boolClass);

	boolClass->addBinop("==", boolClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateICmpEQ(a,b,"andtmp");
	},boolClass);

	boolClass->preops["!"] = new ouopNative(
			[](DATA a, RData& m) -> DATA{
				return m.builder.CreateNot(a,"nottmp");
	},boolClass);

	///////******************************* Double/Double ******************************////////
	doubleClass->addBinop("+", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFAdd(a,b,"addtmp");
	},doubleClass);

	doubleClass->addBinop("-", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFSub(a,b,"subtmp");
	},doubleClass);

	doubleClass->addBinop("*", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFMul(a,b,"multmp");
	},doubleClass);

	doubleClass->addBinop("%", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFRem(a,b,"modtmp");
	},doubleClass);

	doubleClass->addBinop("<", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpULT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop(">", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpUGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop("<=", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpULE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop(">=", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpUGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop("==", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpUEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop("!=", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFCmpUNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	doubleClass->addBinop("/", doubleClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateFDiv(a,b,"divtmp");
	},doubleClass);

	doubleClass->preops["-"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return m.builder.CreateFNeg(a,"negtmp");
	},doubleClass);

	doubleClass->preops["+"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return a;
	},doubleClass);


	///////******************************* INT ********************************////////
	intClass->addBinop("&", intClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateAnd(a,b,"andtmp");
	},intClass);

	intClass->addBinop("|", intClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateOr(a,b,"ortmp");
	},intClass);

	intClass->addBinop("^", intClass) = new obinopNative(
			[](DATA a, DATA b, RData& m) -> DATA{
				return m.builder.CreateXor(a,b,"xortmp");
	},intClass);

	intClass->addBinop("+", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateAdd(a,b,"addtmp");
	},intClass);

	intClass->addBinop("-", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateSub(a,b,"subtmp");
	},intClass);

	intClass->addBinop("*", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateMul(a,b,"multmp");
	},intClass);

	intClass->addBinop("%", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateSRem(a,b,"modtmp");
	},intClass);

	intClass->addBinop("<", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpSLT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop(">", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpSGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("<=", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpSLE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop(">=", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpSGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("==", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("!=", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateICmpNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("/", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateSDiv(a,b,"divtmp");
	},intClass);

	intClass->addBinop("<<", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateShl(a,b);
	},intClass);

	intClass->addBinop(">>", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateAShr(a,b);
	},intClass);

	intClass->addBinop(">>>", intClass) = new obinopNative(
				[](DATA a, DATA b, RData& m) -> DATA{
					return m.builder.CreateLShr(a,b);
	},intClass);

	intClass->preops["-"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return m.builder.CreateNeg(a,"negtmp");
	},intClass);

	intClass->preops["+"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
					return a;
	},intClass);

	intClass->preops["~"] = new ouopNative(
				[](DATA a, RData& m) -> DATA{
				return m.builder.CreateNot(a,"negtmp");
	},intClass);
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
