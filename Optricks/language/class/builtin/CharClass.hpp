/*
 * CharClass.hpp
 *
 *  Created on: Mar 15, 2014
 *      Author: Billy
 */

#ifndef CHARCLASS_HPP_
#define CHARCLASS_HPP_

#include "../AbstractClass.hpp"
#include "./LazyClass.hpp"
#include "./VoidClass.hpp"
class CharClass: public AbstractClass{
public:
	inline CharClass(bool b):
		AbstractClass(nullptr,"char", nullptr,PRIMITIVE_LAYOUT,CLASS_CHAR,true,CHARTYPE){
		LANG_M->addClass(PositionID(0,0,"#char"),this);
		assert(c_intClass.getWidth()>=getWidth());
		LANG_M->addFunction(PositionID(0,0,"#char"),"print")->add(
			new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
			nullptr,[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			const auto value = r.builder.CreateZExtOrTrunc(args[0]->evalV(r, id), c_intClass.type);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			r.builder.CreateCall(CU, value);
			return &VOID_DATA;}), PositionID(0,0,"#char"));
		LANG_M->addFunction(PositionID(0,0,"#char"),"println")->add(
			new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
			nullptr,[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args) -> Data*{
			assert(args.size()==1);
			const auto value = r.builder.CreateZExtOrTrunc(args[0]->evalV(r, id), c_intClass.type);
			auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
			//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
			r.builder.CreateCall(CU, value);
			r.builder.CreateCall(CU, ConstantInt::get(c_intClass.type, '\n',false));
			return &VOID_DATA;}), PositionID(0,0,"#char"));
	}
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		illegalLocal(id,s);
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		illegalLocal(id,s);
		exit(1);
	}

	ConstantInt* getOne() const {
		return ConstantInt::get((llvm::IntegerType*)type,(uint64_t)1);
	}
	bool hasLocalData(String s) const override final{
		return false;
	}
	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final{
		illegalLocal(id,s);
		exit(1);
	}
	unsigned getWidth() const{
		return ((IntegerType*)type)->getBitWidth();
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
		assert(a->classType==CLASS_BOOL);
		assert(b->classType==CLASS_BOOL);
		return 0;
		/*
		if(a->classType==CLASS_BOOL)
			return (b->classType==CLASS_BOOL)?(0):(-1);
		else
			return (b->classType==CLASS_BOOL)?(1):(0);*/
	}
	inline static Constant* getValue(char value){
		return ConstantInt::get(CHARTYPE, value);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CHAR;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CHAR;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, Value* valueToCast) const override{
		if(toCast->classType!=CLASS_CHAR) illegalCast(id,toCast);
		return valueToCast;
	}
};

const CharClass charClass(true);



#endif /* CHARCLASS_HPP_ */
