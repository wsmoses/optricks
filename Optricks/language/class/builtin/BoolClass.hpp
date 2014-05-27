/*
 * BoolClass.hpp
 *
 *  Created on: Dec 27, 2013
 *      Author: Billy
 */

#ifndef BOOLCLASS_HPP_
#define BOOLCLASS_HPP_
#include "../AbstractClass.hpp"
#include "./VoidClass.hpp"
#include "./LazyClass.hpp"
#include "./IntClass.hpp"
class BoolClass: public AbstractClass{
public:
	inline BoolClass(bool b):
		AbstractClass(nullptr,"bool", nullptr,PRIMITIVE_LAYOUT,CLASS_BOOL,true,BOOLTYPE){
		LANG_M.addClass(PositionID(0,0,"#int"),this);
		LANG_M.addFunction(PositionID(0,0,"#class"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			auto V = args[0]->evalV(r, id);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
				if(C->isOne())
					for(auto& a: "true"){
						r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
					}
				else
					for(auto& a: "false"){
						r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
					}
			} else {
				llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
				llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("true"), StartBB->getParent());
				llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("false"), StartBB->getParent());
				llvm::BasicBlock* MergeBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("merge"), StartBB->getParent());
				r.builder.CreateCondBr(V, ThenBB, ElseBB);
				r.builder.SetInsertPoint(ThenBB);
				for(auto& a: "true"){
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
				}
				r.builder.CreateBr(MergeBB);
				r.builder.SetInsertPoint(ElseBB);
				for(auto& a: "false"){
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
				}
				r.builder.CreateBr(MergeBB);
				r.builder.SetInsertPoint(MergeBB);
			}
			return &VOID_DATA;}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#class"),"println")->add(
					new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
					[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
					assert(args.size()==1);
					auto V = args[0]->evalV(r, id);
					auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
					if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
						if(C->isOne())
							for(auto& a: "true\n"){
								r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
							}
						else
							for(auto& a: "false\n"){
								r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
							}
					} else {
						llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
						llvm::BasicBlock* ThenBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("true"), StartBB->getParent());
						llvm::BasicBlock* ElseBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("false"), StartBB->getParent());
						llvm::BasicBlock* MergeBB = llvm::BasicBlock::Create(r.lmod->getContext(),llvm::Twine("merge"), StartBB->getParent());
						r.builder.CreateCondBr(V, ThenBB, ElseBB);
						r.builder.SetInsertPoint(ThenBB);
						for(auto& a: "true\n"){
							r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
						}
						r.builder.CreateBr(MergeBB);
						r.builder.SetInsertPoint(ElseBB);
						for(auto& a: "false\n"){
							r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
						}
						r.builder.CreateBr(MergeBB);
						r.builder.SetInsertPoint(MergeBB);
					}
					return &VOID_DATA;}), PositionID(0,0,"#int"));
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		illegalLocal(id,s);
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	unsigned getWidth() const{
		return ((llvm::IntegerType*)type)->getBitWidth();
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return 0;
	}
	inline static llvm::Constant* getValue(bool value){
		if(value)
			return llvm::ConstantInt::getTrue(BOOLTYPE);
		else
			return llvm::ConstantInt::getFalse(BOOLTYPE);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_BOOL || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_BOOL || toCast->classType==CLASS_VOID;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType!=CLASS_BOOL) illegalCast(id,toCast);
		return valueToCast;
	}
};

const BoolClass boolClass(true);

#endif /* BOOLCLASS_HPP_ */
