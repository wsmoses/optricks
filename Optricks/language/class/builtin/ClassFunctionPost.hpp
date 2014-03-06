/*
 * ClassFunctionPost.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: Billy
 */

#ifndef CLASSFUNCTIONPOST_HPP_
#define CLASSFUNCTIONPOST_HPP_
#include "../AbstractClass.hpp"
#include "./ArrayClass.hpp"
#include "./ClassClass.hpp"
#include "./BoolClass.hpp"
#include "./ComplexClass.hpp"
#include "./CPointerClass.hpp"
#include "./FloatClass.hpp"
#include "./FunctionClass.hpp"
#include "./IntClass.hpp"
#include "./LazyClass.hpp"
#include "./NamedTupleClass.hpp"
#include "./RationalClass.hpp"
#include "./RealClass.hpp"
#include "./ReferenceClass.hpp"
#include "./TupleClass.hpp"
#include "./VoidClass.hpp"


////////////////////////////////////////////////////////////BOOL/////////////////////////////////////////////////////////

SingleFunction* BoolClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==1){
		if(s==":!"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":!", {AbstractDeclaration(this)},this),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				return new ConstantData(r.builder.CreateNot(Start),boolClass);
			}
			);
			return tmp;
		}
		/*else if(s==":str"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":str",{AbstractDeclaration(this)},stringClass),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
						assert(args.size()==1);
						Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
						BasicBlock* StartBB = r.builder.GetInsertBlock();
						BasicBlock *TrueBB;
						BasicBlock *FalseBB;
						BasicBlock* MergeBB;
						if(ConstantInt* c = dyn_cast<ConstantInt>(Start)){
							if(c->isOne()) return (new StringLiteral("true"));
							else return(new StringLiteral("false"));
						}

						TrueBB = r.CreateBlock("gtrue",StartBB);
						FalseBB = r.CreateBlock("gfalse",StartBB);
						MergeBB = r.CreateBlock("gfalse",TrueBB);
						r.builder.CreateCondBr(Start, TrueBB, FalseBB);
						r.builder.SetInsertPoint(TrueBB);
						Value* t = (new StringLiteral("true"))->castToV(r, stringClass, id);
						TrueBB = r.builder.GetInsertBlock();
						r.builder.CreateBr(MergeBB);

						r.builder.SetInsertPoint(FalseBB);
						Value* f = (new StringLiteral("false"))->castToV(r, stringClass, id);
						FalseBB = r.builder.GetInsertBlock();
						r.builder.CreateBr(MergeBB);

						r.builder.SetInsertPoint(MergeBB);
						PHINode *PN = r.builder.CreatePHI(stringClass->type, 2,"iftmp");
						PN->addIncoming(t, TrueBB);
						PN->addIncoming(f, FalseBB);
						return new ConstantData(PN, stringClass);
					}
			);
			return tmp;
		}*/
		//else if(s==":write") return voidClass;
		else{
			if(s[0]==':') id.error("Cannot find unary pre-operator "+s+" in class 'bool'");
			else id.error("Cannot find local method "+s+"() in class 'bool'");
			exit(1);
		}
	} else if(v.size()==2){
		if(v[0]==nullptr){
			//unary (post) operator
			{
				id.error("Cannot find unary post-operator "+s+" in class 'bool'");
				exit(1);
			}
		}
		auto rt = v[0]->getReturnType();
		if(rt->classType!=CLASS_BOOL){
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'bool' and '"+rt->getName()+"'");
			else id.error("Cannot find local method "+s+"("+rt->getName()+") in class 'bool'");
			exit(1);
		}
		if(s==":&&"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":&&",
							{
									AbstractDeclaration(this),
									AbstractDeclaration(LazyClass::get(this))
							},
							this),
							[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
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
					MergeBB = r.CreateBlock("ifcont",StartBB/*,ElseBB*/);
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
			);
			return tmp;
		} else if(s==":||"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":||",
							{
									AbstractDeclaration(this),
									AbstractDeclaration(LazyClass::get(this))
							},
							this),
							[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
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
			);
			return tmp;
		} else if(s==":&"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":&",{AbstractDeclaration(this),AbstractDeclaration(this)},this),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				Value* End = args[1]->evaluate(r)->castToV(r, boolClass, id);
				return new ConstantData(r.builder.CreateAnd(Start,End), boolClass);
			}
			);
			return tmp;
		} else if(s==":|"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":|",{AbstractDeclaration(this),AbstractDeclaration(this)},this),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				Value* End = args[1]->evaluate(r)->castToV(r, boolClass, id);
				return new ConstantData(r.builder.CreateOr(Start,End), boolClass);
			}
			);
			return tmp;
		} else if(s==":=="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":==",{AbstractDeclaration(this),AbstractDeclaration(this)},this),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				Value* End = args[1]->evaluate(r)->castToV(r, boolClass, id);
				return new ConstantData(r.builder.CreateICmpEQ(Start,End), boolClass);
			}
			);
			return tmp;
		} else if(s==":!="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":!=",{AbstractDeclaration(this),AbstractDeclaration(this)},this),
					[](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, boolClass, id);
				Value* End = args[1]->evaluate(r)->castToV(r, boolClass, id);
				return new ConstantData(r.builder.CreateICmpNE(Start,End), boolClass);
			}
			);
			return tmp;
		}
		//else if(s==":&&=" || s==":||=" || s==":&=" || s==":|="){
		//	return this;
		//}
		else{
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'bool' and 'bool'");
			else id.error("Cannot find local method "+s+"(bool) in class 'bool'");
			exit(1);
		}
	} else {
		if(s[0]==':') id.error("Class 'bool' supports only binary and unary operators");
		else id.error("Cannot apply function "+s+" to class 'bool'");
		exit(1);
	}
}

////////////////////////////////////////////////////////////CLASS/////////////////////////////////////////////////////////

SingleFunction* ClassClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==1){
		/*if(s==":str"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
				new FunctionProto(":str",{AbstractDeclaration(this)},stringClass),
				[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==1);
					Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
					if(ConstantInt* ac = dyn_cast<ConstantInt>(Start)){
						const AbstractClass* abc = (const AbstractClass*)(ac->getValue().getLimitedValue());
						return new StringLiteral(abc->getName());
					}
					id.error("Cannot get string of non-constant class");
					exit(1);
				});
			return tmp;
		}
		else
		 */{
			 if(s[0]==':') id.error("Cannot find unary pre-operator "+s+" in class 'class'");
			 else id.error("Cannot find local method "+s+"() in class 'class'");
			 exit(1);
		 }
	} else if(v.size()==2){
		if(v[0]==nullptr){
			//unary (post) operator
			{
				id.error("Cannot find unary post-operator "+s+" in class 'class'");
				exit(1);
			}
		}
		auto rt = v[0]->getReturnType();
		if(rt->classType!=CLASS_CLASS){
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'class' and '"+rt->getName()+"'");
			else id.error("Cannot find local method "+s+"("+rt->getName()+") in class 'class'");
			exit(1);
		}
		if(s==":=="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":==",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpEQ(Start,End),boolClass);
			});
			return tmp;
		}
		else if(s==":!="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":!=",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpNE(Start,End),boolClass);
			});
			return tmp;
		}
		else{
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'class' and 'class'");
			else id.error("Cannot find local method "+s+"(class) in class 'class'");
			exit(1);
		}
	} else {
		if(s[0]==':') id.error("Class 'class' supports only binary and unary operators");
		else id.error("Cannot apply function "+s+" to class 'class'");
		exit(1);
	}
}

////////////////////////////////////////////////////////////C_POINTER/////////////////////////////////////////////////////////

SingleFunction* CPointerClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==1){
		/*if(s==":str"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
				new FunctionProto(":str",{AbstractDeclaration(this)},stringClass),
				[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==1);
					Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
					if(ConstantInt* ac = dyn_cast<ConstantInt>(Start)){
						const AbstractClass* abc = (const AbstractClass*)(ac->getValue().getLimitedValue());
						return new StringLiteral(abc->getName());
					}
					id.error("Cannot get string of non-constant class");
					exit(1);
				});
			return tmp;
		}
		else*/
		{
			if(s[0]==':') id.error("Cannot find unary pre-operator "+s+" in class '"+getName()+"'");
			else id.error("Cannot find local method "+s+"() in class '"+getName()+"'");
			exit(1);
		}
	} else if(v.size()==2){
		if(v[0]==nullptr){
			//unary (post) operator
			{
				id.error("Cannot find unary post-operator "+s+" in class '"+getName()+"'");
				exit(1);
			}
		}
		auto rt = v[0]->getReturnType();
		if(rt->classType!=CLASS_CPOINTER){
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between '"+getName()+"' and '"+rt->getName()+"'");
			else id.error("Cannot find local method "+s+"("+rt->getName()+") in class '"+getName()+"'");
			exit(1);
		}
		if(s==":=="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":==",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpEQ(Start,End),boolClass);
			});
			return tmp;
		}
		else if(s==":!="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":!=",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpNE(Start,End),boolClass);
			});
			return tmp;
		}
		else{
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between '"+getName()+"' and '"+getName()+"'");
			else id.error("Cannot find local method "+s+"(class) in class '"+getName()+"'");
			exit(1);
		}
	} else {
		if(s[0]==':') id.error("Class '"+getName()+"' supports only binary and unary operators");
		else id.error("Cannot apply function "+s+" to class '"+getName()+"'");
		exit(1);
	}
}

////////////////////////////////////////////////////////////INT_CLASS/////////////////////////////////////////////////////////

SingleFunction* IntClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==1){
		if(s==":+"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":+",{AbstractDeclaration(this)},this),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				return args[0]->evaluate(r)->toValue(r, id);
			});
			return tmp;
		}
		else if(s==":-"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":-",{AbstractDeclaration(this)},this),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				Value* v = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateNeg(v), this);
			});
			return tmp;
		}
		else if(s==":~"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":~",{AbstractDeclaration(this)},this),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				Value* v = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateNot(v), this);
			});
			return tmp;
		}
		else if(s==":++"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":++",{AbstractDeclaration(ReferenceClass::get(this))},this),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				auto tmp = args[0]->evaluate(r);
				assert(tmp->type==R_LOC);
				auto L = ((LocationData*)tmp)->value;
				Value* toR = L->getValue(r, id);
				L->setValue(r.builder.CreateAdd(toR, getOne()), r);
				return new ConstantData(toR, this);
				//auto loc = ((LocationData*)tmp)->value->getPointer(r, id);
				//return new ConstantData(r.builder.CreateAtomicRMW(AtomicRMWInst::BinOp::Add, loc, getOne(),llvm::AtomicOrdering::NotAtomic), this);
			});
			return tmp;
		}
		else if(s==":--"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":--",{AbstractDeclaration(ReferenceClass::get(this))},this),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==1);
				auto tmp = args[0]->evaluate(r);
				assert(tmp->type==R_LOC);
				auto L = ((LocationData*)tmp)->value;
				Value* toR = L->getValue(r, id);
				L->setValue(r.builder.CreateSub(toR, getOne()), r);
				return new ConstantData(toR, this);
				//auto loc = ((LocationData*)tmp)->value->getPointer(r, id);
				//return new ConstantData(r.builder.CreateAtomicRMW(AtomicRMWInst::BinOp::Sub, loc, getOne(),llvm::AtomicOrdering::NotAtomic), this);
			});
			return tmp;
		}
		//if(s==":str") return stringClass;
		else
		{
			if(s[0]==':') id.error("Cannot find unary pre-operator "+s+" in class '"+getName()+"'");
			else id.error("Cannot find local method "+s+"() in class '"+getName()+"'");
			exit(1);
		}
	} else if(v.size()==2){
		if(v[0]==nullptr){
			if(s==":++"){
				//unary (post) operatorif(s==":++"){
				static SingleFunction* tmp = new BuiltinInlineFunction(
						new FunctionProto(":++",{AbstractDeclaration(ReferenceClass::get(this))},this),
						[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==1);
					auto tmp = args[0]->evaluate(r);
					assert(tmp->type==R_LOC);
					auto L = ((LocationData*)tmp)->value;
					Value* toR = L->getValue(r, id);
					Value* toS = r.builder.CreateAdd(toR, getOne());
					L->setValue(toS, r);
					return new ConstantData(toS, this);
				});
				return tmp;
			}
			else if(s==":--"){
				static SingleFunction* tmp = new BuiltinInlineFunction(
						new FunctionProto(":--",{AbstractDeclaration(ReferenceClass::get(this))},this),
						[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==1);
					auto tmp = args[0]->evaluate(r);
					assert(tmp->type==R_LOC);
					auto L = ((LocationData*)tmp)->value;
					Value* toR = L->getValue(r, id);
					Value* toS = r.builder.CreateSub(toR, getOne());
					L->setValue(toS, r);
					return new ConstantData(toS, this);
				});
				return tmp;
			}
			else {
				id.error("Cannot find unary post-operator "+s+" in class '"+getName()+"'");
				exit(1);
			}
		}
		auto rt = v[0]->getReturnType();
		switch(rt->classType){
		case CLASS_INT:{
			const IntClass* max = (IntClass*)rt;
			if(max->getWidth()<=getWidth()){
				max = this;
			}
			if(s==":=="){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":==",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpEQ(t1, t2), max);
				});
				return tmp;
			} else if(s==":!="){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":!=",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpNE(t1, t2), max);
				});
				return tmp;
			} else if(s==":>"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":>",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpSGT(t1, t2), max);
				});
				return tmp;
			} else if(s==":>="){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":>=",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpSGE(t1, t2), max);
				});
				return tmp;
			} else if(s==":<"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":<",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpSLT(t1, t2), max);
				});
				return tmp;
			} else if(s==":<="){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":<=",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateICmpSLE(t1, t2), max);
				});
				return tmp;
			} else if(s==":+"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":+",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateAdd(t1, t2), max);
				});
				return tmp;
			} else if(s==":-"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":-",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateSub(t1, t2), max);
				});
				return tmp;
			} else if(s==":*"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":*",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateMul(t1, t2), max);
				});
				return tmp;
			} else if(s==":/"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":/",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateSDiv(t1, t2), max);
				});
				return tmp;
			} else if(s==":%"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":%",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateSRem(t1, t2), max);
				});
				return tmp;
			} else if(s==":&"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":&",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateAnd(t1, t2), max);
				});
				return tmp;
			} else if(s==":|"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":|",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateOr(t1, t2), max);
				});
				return tmp;
			} else if(s==":<<"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":<<",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateShl(t1, t2), max);
				});
				return tmp;
			} else if(s==":>>"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":>>",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateAShr(t1, t2), max);
				});
				return tmp;
			} else if(s==":>>>"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":>>>",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					auto t1 = args[0]->evaluate(r)->castToV(r, max, id);
					auto t2 = args[1]->evaluate(r)->castToV(r, max, id);
					return new ConstantData(r.builder.CreateLShr(t1, t2), max);
				});
				return tmp;
			} else if(s==":**"){
				static std::map<const IntClass*, SingleFunction*> tmp;
				auto found = tmp.find(max);
				if(found!=tmp.end()) return found->second;
				else return tmp[max] = new BuiltinInlineFunction(
						new FunctionProto(":**",{AbstractDeclaration(this),AbstractDeclaration(rt)},this),
						[max](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==2);
					Value *a = args[0]->getValue(r,id);
					Value *b = args[1]->getValue(r,id);
					if(ConstantInt* d = dyn_cast<ConstantInt>(b)){
						auto b1 = d->getValue().getSExtValue();
						if(ConstantInt* c = dyn_cast<ConstantInt>(a)){
							auto a1 = c->getValue();
							if(b1<0){
								id.warning()
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
					BasicBlock *ThenBB = m.CreateBlockD("if_zero", TheFunction);
					BasicBlock *ElseBB = m.CreateBlockD("_nonzero", TheFunction);
					BasicBlock *MergeBB = m.CreateBlockD("all_pow", TheFunction);
					m.builder.CreateCondBr(m.builder.CreateICmpEQ(b,getInt(0)), ThenBB, ElseBB);
					m.builder.SetInsertPoint(ThenBB);
					double temp1[2] = {1,0};
					Value* ifZero = ConstantDataVector::get(m.lmod->getContext(), ArrayRef<double>(temp1));
					m.builder.CreateBr(MergeBB);
					//INCOMING ifZero MergeBB
					m.builder.SetInsertPoint(ElseBB);
					BasicBlock *LessBB = m.CreateBlockD("_lesszero", TheFunction);
					BasicBlock *PreLoopBB = m.CreateBlockD("_preLoop", TheFunction);
					BasicBlock *GreatBB = m.CreateBlockD("_greatzero", TheFunction);
					m.builder.CreateCondBr(m.builder.CreateICmpSLT(b, getInt(0)), LessBB, PreLoopBB);
					m.builder.SetInsertPoint(PreLoopBB);
					Value* subOne = m.builder.CreateSub(b, getInt(1));
					m.builder.CreateCondBr(m.builder.CreateICmpEQ(subOne, getInt(0)), MergeBB, GreatBB);
					m.builder.SetInsertPoint(LessBB);
					Value* inversed = complexInverse(a,m);
					Value* realExp = m.builder.CreateSub(getInt(-1), b);
					m.builder.CreateCondBr(m.builder.CreateICmpEQ(realExp,getInt(0)), MergeBB,GreatBB);
					m.builder.SetInsertPoint(GreatBB);
					PHINode* X = m.CreatePHI(COMPLEXTYPE, 3, "temp");
					X->addIncoming(inversed, LessBB);
					X->addIncoming(a, PreLoopBB);
					//X->addIncoming();
					PHINode* PN = m.CreatePHI(COMPLEXTYPE, 3, "result");
					PN->addIncoming(inversed, LessBB);
					PN->addIncoming(a, PreLoopBB);
					//PN->addIncoming();
					PHINode* exp = m.CreatePHI(INTTYPE, 3, "pow");
					exp->addIncoming(realExp, LessBB);
					exp->addIncoming(subOne, PreLoopBB);
					//exp->addIncoming();
					BasicBlock *OddBB = m.CreateBlockD("if_odd", TheFunction);
					BasicBlock *EvenBB = m.CreateBlockD("if_even", TheFunction);
					m.builder.CreateCondBr(m.builder.CreateTrunc(exp, BOOLTYPE), OddBB, EvenBB);
					m.builder.SetInsertPoint(OddBB);
					Value* resultOdd = complexMultiply(PN, X, m);
					m.builder.CreateCondBr(m.builder.CreateICmpEQ(exp, getInt(1)), MergeBB, EvenBB);
					//INCOMING resuldOdd to MergeBB;
					m.builder.SetInsertPoint(EvenBB);
					PHINode* res = m.CreatePHI(COMPLEXTYPE, 2, "res2");
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
					PHINode* comp = m.CreatePHI(COMPLEXTYPE, 4, "finalExp");
					comp->addIncoming(ifZero, ThenBB);
					comp->addIncoming(resultOdd, OddBB);
					comp->addIncoming(inversed, LessBB);
					comp->addIncoming(a, PreLoopBB);
					//comp->addIncoming(res, EvenBB);
					return DATA::getConstant(comp,complexClass);
				});
				return tmp;
			}

		}
		}
		fdsafdas;//todo finish this
		if(rt->classType!=CLASS_INT && rt->classType!=CLASS_FLOAT && rt->classType!=CLASS_COMPLEX && rt->classType!=CLASS_INTLITERAL && rt->classType!=CLASS_FLOATLITERAL){
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between '"+getName()+"' and '"+rt->getName()+"'");
			else id.error("Cannot find local method "+s+"("+rt->getName()+") in class '"+getName()+"'");
			exit(1);
		}
		if(s==":==" || s==":!="){
			return boolClass;
		} else if(s==":+" || s==":-" || s==":/" || s==":*" || s==":**" || s==":%" || s==":&" || s==":|" || s==":^"){
			return getMin(rt, this, id);
		} else if(s==":<<" || s==":>>" || s==":<<<"){
			if(rt->classType!=CLASS_INT && rt->classType!=CLASS_INTLITERAL){
				id.error("Cannot find binary operator "+s+" between '"+getName()+"' and '"+rt->getName()+"'");
				exit(1);
			}
		}
		else{
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between '"+getName()+"' and '"+getName()+"'");
			else id.error("Cannot find local method "+s+"("+getName()+") in class '"+getName()+"'");
			exit(1);
		}
	} else {
		if(s[0]==':') id.error("Class '"+getName()+"' supports only binary and unary operators");
		else id.error("Cannot apply function "+s+" to class '"+getName()+"'");
		exit(1);
	}
}


////////////////////////////////////////////////////////////ARRAY/////////////////////////////////////////////////////////

SingleFunction* ArrayClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==1){
		/*if(s==":str"){
			static SingleFunction* tmp = new BuiltinInlineFunction(
				new FunctionProto(":str",{AbstractDeclaration(this)},stringClass),
				[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
					assert(args.size()==1);
					Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
					if(ConstantInt* ac = dyn_cast<ConstantInt>(Start)){
						const AbstractClass* abc = (const AbstractClass*)(ac->getValue().getLimitedValue());
						return new StringLiteral(abc->getName());
					}
					id.error("Cannot get string of non-constant class");
					exit(1);
				});
			return tmp;
		}
		else
		 */{
			 if(s[0]==':') id.error("Cannot find unary pre-operator "+s+" in class 'class'");
			 else id.error("Cannot find local method "+s+"() in class 'class'");
			 exit(1);
		 }
	} else if(v.size()==2){
		if(v[0]==nullptr){
			//unary (post) operator
			{
				id.error("Cannot find unary post-operator "+s+" in class 'class'");
				exit(1);
			}
		}
		auto rt = v[0]->getReturnType();
		if(rt->classType!=CLASS_CLASS){
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'class' and '"+rt->getName()+"'");
			else id.error("Cannot find local method "+s+"("+rt->getName()+") in class 'class'");
			exit(1);
		}
		if(s==":=="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":==",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpEQ(Start,End),boolClass);
			});
			return tmp;
		}
		else if(s==":!="){
			static SingleFunction* tmp = new BuiltinInlineFunction(
					new FunctionProto(":!=",{AbstractDeclaration(this),AbstractDeclaration(this)},boolClass),
					[this](RData& r, PositionID id, const std::vector<const Evaluatable*>& args) -> const Data*{
				assert(args.size()==2);
				Value* Start = args[0]->evaluate(r)->castToV(r, this, id);
				Value* End = args[0]->evaluate(r)->castToV(r, this, id);
				return new ConstantData(r.builder.CreateICmpNE(Start,End),boolClass);
			});
			return tmp;
		}
		else{
			if(s[0]==':') id.error("Cannot find binary operator "+s+" between 'class' and 'class'");
			else id.error("Cannot find local method "+s+"(class) in class 'class'");
			exit(1);
		}
	} else {
		if(s[0]==':') id.error("Class 'class' supports only binary and unary operators");
		else id.error("Cannot apply function "+s+" to class 'class'");
		exit(1);
	}
}
#endif /* CLASSFUNCTIONPOST_HPP_ */
