/*
 * CStringClass.hpp
 *
 *  Created on: May 25, 2014
 *      Author: Billy
 */

#ifndef CSTRINGCLASS_HPP_
#define CSTRINGCLASS_HPP_


#include "../AbstractClass.hpp"
#include "../ScopeClass.hpp"
class CStringClass: public AbstractClass{
public:
	inline CStringClass(bool b):
		AbstractClass(& NS_LANG_C.staticVariables,"string", NULL,PRIMITIVE_LAYOUT,CLASS_CSTRING,true,C_STRINGTYPE){
		NS_LANG_C.staticVariables.addClass(PositionID(0,0,"#cstring"),this);
		/*
		LANG_M.addFunction(PositionID(0,0,"#stringL"),"print")->add(
						new BuiltinInlineFunction(new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						const auto& value = ((const StringLiteral*) args[0]->evaluate(r))->value;
						auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
						//auto CU = r.getExtern("putchar_unlocked", &c_intClass, {&c_intClass});
						for(const auto& a: value){
							r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
						}
						return &VOID_DATA;}), PositionID(0,0,"#int"));*/

		LANG_M.addFunction(PositionID(0,0,"#str"),"print")->add(
						new BuiltinInlineFunction(
								new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
						[](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						r.printf("%s", args[0]->evalV(r, id));
						return &VOID_DATA;
					}), PositionID(0,0,"#int"));
		LANG_M.addFunction(PositionID(0,0,"#cstr"), "println")->add(
				new CompiledFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
						rdata.getExtern("puts",&c_intClass, {this})), PositionID(0,0,"#cstr"));
		/*
				LANG_M.addFunction(PositionID(0,0,"#cstr"),"println")->add(
					new BuiltinInlineFunction(new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
					[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
					assert(args.size()==1);
					llvm::Value* value = args[0]->evalV(r, id);
					auto CU = r.getExtern("puts", &c_intClass, {this});
					r.builder.CreateCall(CU, value);
				return &VOID_DATA;}), PositionID(0,0,"#cstr"));
		*/
	}

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
	/*
	std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
			illegalLocal(id,s);
			return std::pair<AbstractClass*,unsigned int>(this,0);
		}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CSTRING || toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		return toCast->classType==CLASS_CSTRING || toCast->classType==CLASS_CPOINTER || toCast->classType==CLASS_VOID;
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const override final{
			assert(hasCast(a));
			assert(hasCast(b));
			if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
			else if(a->classType==CLASS_VOID) return 1;
			else if(b->classType==CLASS_VOID) return -1;
			if(a->classType==CLASS_CPOINTER && b->classType==CLASS_CPOINTER) return 0;
			else if(a->classType==CLASS_CPOINTER) return 1;
			else if(b->classType==CLASS_CPOINTER) return -1;
			assert(a->classType==CLASS_CSTRING);
			assert(b->classType==CLASS_CSTRING);
			return 0;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(toCast->classType==CLASS_CPOINTER) return r.pointerCast(valueToCast, C_POINTERTYPE);
		if(toCast->classType!=CLASS_CSTRING) id.error("Cannot cast "+getName()+" type to "+toCast->getName());
		return valueToCast;
	}
};

const CStringClass c_stringClass(true);



#endif /* CSTRINGCLASS_HPP_ */
