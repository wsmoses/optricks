/*
 * operators.hpp
 *
 *  Created on: Aug 27, 2013
 *      Author: wmoses
 */

#ifndef OPERATORS_HPP_
#define OPERATORS_HPP_


bool isStartName(int i){
	return isalpha(i) || i=='_' || i=='$';
}
void initClassesMeta(){
	intClass->addCast(decClass) = new ouopNative(
			[](Value* a, RData& m) -> Value*{
				return m.builder.CreateSIToFP(a,DOUBLETYPE);
	}
	,decClass);
	///////******************************* Boolean ********************************////////
	/* //TODO short-circuit
	boolClass->binops["&&"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->binops["||"][boolClass] = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);
	*/
	boolClass->addBinop("&", boolClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},boolClass);

	boolClass->addBinop("|", boolClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},boolClass);

	boolClass->addBinop("^", boolClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateXor(a,b,"xortmp");
	},boolClass);

	boolClass->addBinop("!=", boolClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpNE(a,b,"andtmp");
	},boolClass);

	boolClass->addBinop("==", boolClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateICmpEQ(a,b,"andtmp");
	},boolClass);

	boolClass->preops["!"] = new ouopNative(
			[](Value* a, RData& m) -> Value*{
				return m.builder.CreateNot(a,"nottmp");
	},boolClass);

	///////******************************* Double/Double ******************************////////
	decClass->addBinop("+", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(a,b,"addtmp");
	},decClass);

	decClass->addBinop("-", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(a,b,"subtmp");
	},decClass);

	decClass->addBinop("*", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(a,b,"multmp");
	},decClass);

	decClass->addBinop("%", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(a,b,"modtmp");
	},decClass);

	decClass->addBinop("<", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop(">", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop("<=", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop(">=", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop("==", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop("!=", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	decClass->addBinop("/", decClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(a,b,"divtmp");
	},decClass);

	decClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateFNeg(a,"negtmp");
	},decClass);

	decClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},decClass);


	///////******************************* INT ********************************////////
	intClass->addBinop("&", intClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateAnd(a,b,"andtmp");
	},intClass);

	intClass->addBinop("|", intClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateOr(a,b,"ortmp");
	},intClass);

	intClass->addBinop("^", intClass) = new obinopNative(
			[](Value* a, Value* b, RData& m) -> Value*{
				return m.builder.CreateXor(a,b,"xortmp");
	},intClass);

	intClass->addBinop("+", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateAdd(a,b,"addtmp");
	},intClass);

	intClass->addBinop("-", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSub(a,b,"subtmp");
	},intClass);

	intClass->addBinop("*", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateMul(a,b,"multmp");
	},intClass);

	intClass->addBinop("%", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSRem(a,b,"modtmp");
	},intClass);

	intClass->addBinop("<", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop(">", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGT(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("<=", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSLE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop(">=", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpSGE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("==", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpEQ(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("!=", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateICmpNE(a,b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->addBinop("/", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateSDiv(a,b,"divtmp");
	},intClass);

	intClass->addBinop("<<", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateShl(a,b);
	},intClass);

	intClass->addBinop(">>", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateAShr(a,b);
	},intClass);

	intClass->addBinop(">>>", intClass) = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateLShr(a,b);
	},intClass);

	intClass->preops["-"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return m.builder.CreateNeg(a,"negtmp");
	},intClass);

	intClass->preops["+"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
					return a;
	},intClass);

	intClass->preops["~"] = new ouopNative(
				[](Value* a, RData& m) -> Value*{
				return m.builder.CreateNot(a,"negtmp");
	},intClass);

	///////******************************* INT/Double ********************************////////
	/*
	intClass->binops["+"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFAdd(m.builder.CreateSIToFP(a,b->getType()),b,"addtmp");
	},decClass);

	intClass->binops["-"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFSub(m.builder.CreateSIToFP(a,b->getType()),b,"subtmp");
	},decClass);

	intClass->binops["*"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFMul(m.builder.CreateSIToFP(a,b->getType()),b,"multmp");
	},decClass);

	intClass->binops["%"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFRem(m.builder.CreateSIToFP(a,b->getType()),b,"modtmp");
	},decClass);

	intClass->binops["<"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGT(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["<="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpULE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops[">="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUGE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["=="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUEQ(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["!="][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFCmpUNE(m.builder.CreateSIToFP(a,b->getType()),b,"cmptmp");
					//TODO there is also a CreateFCmpOGT??
	},boolClass);

	intClass->binops["/"][decClass] = new obinopNative(
				[](Value* a, Value* b, RData& m) -> Value*{
					return m.builder.CreateFDiv(m.builder.CreateSIToFP(a,b->getType()),b,"divtmp");
	},decClass);
	*/
	///////******************************* DOUBLE/int ********************************////////
	/*
	decClass->binops["+"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFAdd(a,m.builder.CreateSIToFP(b,a->getType()),"addtmp");
		},decClass);

		decClass->binops["-"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFSub(a,m.builder.CreateSIToFP(b,a->getType()),"subtmp");
		},decClass);

		decClass->binops["*"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFMul(a,m.builder.CreateSIToFP(b,a->getType()),"multmp");
		},decClass);

		decClass->binops["%"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFRem(a,m.builder.CreateSIToFP(b,a->getType()),"modtmp");
		},decClass);

		decClass->binops["<"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpULT(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops[">"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUGT(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["<="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpULE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops[">="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUGE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["=="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUEQ(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["!="][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFCmpUNE(a,m.builder.CreateSIToFP(b,a->getType()),"cmptmp");
						//TODO there is also a CreateFCmpOGT??
		},boolClass);

		decClass->binops["/"][intClass] = new obinopNative(
					[](Value* a, Value* b, RData& m) -> Value*{
						return m.builder.CreateFDiv(a,m.builder.CreateSIToFP(b,a->getType()),"divtmp");
		},decClass);
		*/
	/*
	LANG_M->addPointer("class",classClass,0);
	LANG_M->addPointer("object",objectClass,0);
	LANG_M->addPointer("bool",boolClass,0);
	LANG_M->addPointer("array",arrayClass,0);
	LANG_M->addPointer("function",functionClass,0);
	LANG_M->addPointer("int",intClass,0);
	LANG_M->addPointer("double",decClass,0);
	LANG_M->addPointer("string",stringClass,0);
	LANG_M->addPointer("slice",sliceClass,0);*/
}



#endif /* OPERATORS_HPP_ */
