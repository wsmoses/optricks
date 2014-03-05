/*
 * BinaryOperations.hpp
 *
 *  Created on: Feb 12, 2014
 *      Author: Billy
 */

#ifndef OPERATORS_HPP_
#define OPERATORS_HPP_
#include "./function/AbstractFunction.hpp"
/*
std::map<String,OverloadedFunction> OPERATOR_MAP;

inline void addOperator(String s, SingleFunction* f, PositionID id){
	auto find = OPERATOR_MAP.find(s);
	if(find==OPERATOR_MAP.end()){
		find = OPERATOR_MAP.insert(std::pair<String, OverloadedFunction>(s, OverloadedFunction(s))).first;
	}
	find->second.add(f, id);
}

void initFunctions(RData& m){
	PositionID func("#init",0,0);
	addOperator("&&", new BuiltinInlineFunction(
			new FunctionProto(":&&",
					{
							AbstractDeclaration(LazyClass::get(boolClass)),
							AbstractDeclaration(LazyClass::get(boolClass))
					},
			boolClass),
			[](RData& r, PositionID id, const std::vector<Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				BasicBlock* StartBB = r.builder.GetInsertBlock();
				BasicBlock *ElseBB;
				BasicBlock *MergeBB;
				if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
					if(c->isOne()) return args[1]->evaluate(r)->castTo(r,boolClass,id);
					else return new BoolLiteral(false);
				}
				else{
					ElseBB = r.CreateBlock("else",StartBB);
					MergeBB = r.CreateBlock("ifcont",StartBB);
					r.builder.CreateCondBr(Start, ElseBB, MergeBB);
				}
				r.builder.SetInsertPoint(ElseBB);
				Value* fin = args[1]->evaluate(r)->castToV(r,boolClass,id);
				//TODO can allow check if right is constant
				r.builder.CreateBr(MergeBB);
				ElseBB = r.builder.GetInsertBlock();
				//a.addPred(MergeBB,ElseBB);
				r.builder.SetInsertPoint(MergeBB);
				PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
				PN->addIncoming(Start, StartBB);
				PN->addIncoming(fin, ElseBB);
				return new ConstantData(PN, boolClass);
			}
	),func);

	addOperator("||",new BuiltinInlineFunction(
			new FunctionProto(":||",
					{
							AbstractDeclaration(LazyClass::get(boolClass)),
							AbstractDeclaration(LazyClass::get(boolClass))
					},
			boolClass),
			[](RData& r, PositionID id, const std::vector<Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				BasicBlock* StartBB = r.builder.GetInsertBlock();
				BasicBlock *ElseBB;
				BasicBlock *MergeBB;
				if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
					if(!c->isOne()) return args[1]->evaluate(r)->castTo(r,boolClass,id);
					else return new BoolLiteral(true);
				}
				else{
					ElseBB = r.CreateBlock("else",StartBB);
					MergeBB = r.CreateBlock("ifcont",StartBB);
					r.builder.CreateCondBr(Start, MergeBB, ElseBB);
				}
				StartBB = r.builder.GetInsertBlock();
				r.builder.SetInsertPoint(ElseBB);
				Value* fin = args[1]->evaluate(r)->castToV(r,boolClass,id);
				//TODO can allow check if right is constant
				r.builder.CreateBr(MergeBB);
				ElseBB = r.builder.GetInsertBlock();
				//a.addPred(MergeBB,ElseBB);
				r.builder.SetInsertPoint(MergeBB);
				PHINode *PN = r.builder.CreatePHI(BOOLTYPE, 2,"iftmp");
				PN->addIncoming(Start, StartBB);
				PN->addIncoming(fin, ElseBB);
				return new ConstantData(PN, boolClass);
			}
	),func);
}

*/
#endif /* OPERATORS_HPP_ */
