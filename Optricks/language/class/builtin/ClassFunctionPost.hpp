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

/*
////////////////////////////////////////////////////////////BOOL/////////////////////////////////////////////////////////

SingleFunction* BoolClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==2){
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

////////////////////////////////////////////////////////////C_POINTER/////////////////////////////////////////////////////////

SingleFunction* CPointerClass::getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
	assert(s.size()>0);
	if(v.size()==2){
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
	if(v.size()==2){
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
			} else if(s==":**"){
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
*/
#endif /* CLASSFUNCTIONPOST_HPP_ */
