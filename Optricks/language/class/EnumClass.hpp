/*
 * EnumClass.hpp
 *
 *  Created on: May 27, 2014
 *      Author: Billy
 */

#ifndef ENUMCLASS_HPP_
#define ENUMCLASS_HPP_

#include "./AbstractClass.hpp"
#include "../function/UnaryFunction.hpp"

class EnumClass: public AbstractClass{
private:
	std::map<String, OverloadedFunction*> localFunctions;
	std::map<String,UnaryFunction> preop;
	std::map<String,UnaryFunction> postop;
public:
	mutable OverloadedFunction constructors;
public:
	friend AbstractClass;
	const std::vector<String> names;
	EnumClass(const Scopable* sc, String nam, const std::vector<String>& n)
	: AbstractClass(sc,nam,nullptr,PRIMITIVE_LAYOUT,CLASS_ENUM,true,llvm::Type::getInt32Ty(llvm::getGlobalContext()))
,constructors(nam, nullptr),names(n){
		for(unsigned i=0; i<names.size(); i++)
			this->staticVariables.addVariable(PositionID(0,0,"#enum"),names[i],new ConstantData(getInt32(i),this));

		LANG_M.addFunction(PositionID(0,0,"#enum"),"print")->add(
						new BuiltinInlineFunction(
								new FunctionProto("print",{AbstractDeclaration(this)},&voidClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
						assert(args.size()==1);
						llvm::Value* V = args[0]->evalV(r, id);
						if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
							auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
							for(const auto& a: this->names[C->getValue().getLimitedValue()]){
								r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
							}
						} else {
							llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
							llvm::BasicBlock* ErrorBB = r.CreateBlock("arrayError",StartBB);
							llvm::BasicBlock* MergeBB = r.CreateBlock("arrayMerge");
							auto Switch = r.builder.CreateSwitch(V, ErrorBB, names.size());
							r.builder.SetInsertPoint(MergeBB);
							auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
							for(unsigned i=0; i<names.size(); i++){
								llvm::BasicBlock* TmpBB = r.CreateBlock("arrayPiece",StartBB);
								r.builder.SetInsertPoint(TmpBB);
								for(const auto& a: this->names[i]){
									r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
								}
								r.builder.CreateBr(MergeBB);
								Switch->addCase(getInt32(i), TmpBB);
							}
							r.builder.SetInsertPoint(MergeBB);
						}
						return &VOID_DATA;
		}), PositionID(0,0,"#enum"));
		LANG_M.addFunction(PositionID(0,0,"#enum"),"println")->add(
				new BuiltinInlineFunction(
								new FunctionProto("println",{AbstractDeclaration(this)},&voidClass),
						[=](RData& r,PositionID id,const std::vector<const Evaluatable*>& args,const Data* instance) -> Data*{
				assert(args.size()==1);
				llvm::Value* V = args[0]->evalV(r, id);
				if(auto C = llvm::dyn_cast<llvm::ConstantInt>(V)){
					auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
					for(const auto& a: this->names[C->getValue().getLimitedValue()]){
						r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
					}
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, '\n',false));
				} else {
					llvm::BasicBlock* StartBB = r.builder.GetInsertBlock();
					llvm::BasicBlock* ErrorBB = r.CreateBlock("arrayError",StartBB);
					llvm::BasicBlock* MergeBB = r.CreateBlock("arrayMerge");
					auto Switch = r.builder.CreateSwitch(V, ErrorBB, names.size());
					r.builder.SetInsertPoint(MergeBB);
					auto CU = r.getExtern("putchar", &c_intClass, {&c_intClass});
					for(unsigned i=0; i<names.size(); i++){
						llvm::BasicBlock* TmpBB = r.CreateBlock("arrayPiece",StartBB);
						r.builder.SetInsertPoint(TmpBB);
						for(const auto& a: this->names[i]){
							r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, a,false));
						}
						r.builder.CreateBr(MergeBB);
						Switch->addCase(getInt32(i), TmpBB);
					}
					r.builder.SetInsertPoint(MergeBB);
					r.builder.CreateCall(CU, llvm::ConstantInt::get(c_intClass.type, '\n',false));
				}
				return &VOID_DATA;
		}), PositionID(0,0,"#enum"));
	};
	inline OverloadedFunction* addLocalFunction(const String s, void* generic=nullptr){
		auto find = localFunctions.find(s);
		if(find==localFunctions.end()){
			return localFunctions.insert(
					std::pair<String,OverloadedFunction*>(s,new OverloadedFunction(s, generic))).first->second;
		}
		return find->second;
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
	inline const UnaryFunction& getPostop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->postop.find(s);
			if(find!=tmp->postop.end()) return find->second;
			tmp = (const EnumClass*)superClass;
		}while(tmp);
		id.error("Could not find unary post-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const UnaryFunction& getPreop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->preop.find(s);
			if(find!=tmp->preop.end()) return find->second;
			tmp = (const EnumClass*)superClass;
		}while(tmp);
		id.error("Could not find unary pre-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const AbstractClass*>& v) const{
		id.error("Could not find local method '"+s+"' in enum '"+getName()+"'");
		exit(1);
	}
	inline SingleFunction* getLocalFunction(PositionID id, String s, const std::vector<const Evaluatable*>& v) const{
		id.error("Could not find local method '"+s+"' in enum '"+getName()+"'");
		return nullptr;
		//exit(1);
	}
	bool noopCast(const AbstractClass* const toCast) const override{
		return this==toCast || toCast->classType==CLASS_VOID;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		if(toCast->classType==CLASS_VOID) return true;
		return this==toCast || toCast->classType==CLASS_VOID;
	}
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(!hasCast(toCast)){
			id.error("Cannot promote class '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		return valueToCast;
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;
		return 0;
	}
};

#endif /* ENUMCLASS_HPP_ */
