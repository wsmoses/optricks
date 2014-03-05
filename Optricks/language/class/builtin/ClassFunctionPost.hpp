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
		else if(isSigned && s==":-"){
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
