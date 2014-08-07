/*
 * UserClass.hpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Billy
 */

#ifndef USERCLASS_HPP_
#define USERCLASS_HPP_
#include "./AbstractClass.hpp"
#include "./builtin/ReferenceClass.hpp"
#include "../function/UnaryFunction.hpp"
//TODO note if class has 1 arg, does not make struct
//TODO note if class has 0 arg, does not make anything
class UserClass: public AbstractClass{
private:
	std::map<String, OverloadedFunction*> localFunctions;
	std::vector<const AbstractClass*> localVars;
	std::map<String,unsigned int> localMap;
	std::map<String,UnaryFunction> preop;
	std::map<String,UnaryFunction> postop;
public:
	mutable OverloadedFunction constructors;
private:
	unsigned int start;
	bool final;
protected:
	std::vector<UserClass*> children;
public:
	friend AbstractClass;
	UserClass(const Scopable* sc, String nam, const AbstractClass* const supa, LayoutType t, bool fina,bool isObject=false,llvm::Type* T=nullptr);
	inline void addLocalFunction(const String s, PositionID id, SingleFunction* F, bool forceOverride/*=false*/, void* generic=nullptr){
		assert(F->getSingleProto()->declarations.size()>=1);
		assert(F->getSingleProto()->declarations[0].declarationType==this || (layout==PRIMITIVE_LAYOUT &&
				F->getSingleProto()->declarations[0].declarationType->classType==CLASS_REF &&
				((ReferenceClass*)F->getSingleProto()->declarations[0].declarationType)->innerType==this
		));
		auto find = localFunctions.find(s);
		auto container = (find==localFunctions.end())?localFunctions.insert(
					std::pair<String,OverloadedFunction*>(s,new OverloadedFunction(s, generic))).first->second:
				find->second;
		//TODO VALIDATE USE OF METHOD
		container->add(F, id);
	}
	inline const UnaryFunction& getPostop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->postop.find(s);
			if(find!=tmp->postop.end()) return find->second;
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find unary post-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const UnaryFunction& getPreop(PositionID id, String s) const{
		auto tmp = this;
		do{
			auto find = tmp->preop.find(s);
			if(find!=tmp->preop.end()) return find->second;
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find unary pre-operator '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const Data* callLocalFunction(RData& r, PositionID id, String s, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v, const Data* instance) const{
		assert(instance);
		if(layout!=POINTER_LAYOUT){
			auto tmp = this;
			do{
				auto find = tmp->localFunctions.find(s);
				if(find!=tmp->localFunctions.end()) {
					if(!t_args.inUse)
						return find->second->getBestFit(id,v, true)->callFunction(r, id, v, instance);
					else
						return find->second->getBestFit(id, t_args.eval(r, id), true)->callFunction(r, id, v, instance);
				}
				tmp = (const UserClass*)(tmp->superClass);
			}while(tmp);
			id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
			exit(1);
		} else {
			SingleFunction* SF=nullptr;{
			auto tmp = this;
			do{
				auto find = tmp->localFunctions.find(s);
				if(find!=tmp->localFunctions.end()) {
					if(!t_args.inUse)
						SF = find->second->getBestFit(id, v, true);
					else
						SF = find->second->getBestFit(id, t_args.eval(r, id), true);
					break;
				}
				tmp = (const UserClass*)(tmp->superClass);
			}while(tmp);}
			if(SF==nullptr){
				id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
				exit(1);
			}
			unsigned countClasses = 1;
			unsigned countFunctions = 1;
			std::stack<UserClass*> Q;
			for(const auto& a : children){
				Q.push(a);
			}
			while(Q.size()!=0){
				auto P = Q.top();
				Q.pop();
				auto find = P->localFunctions.find(s);
				countClasses++;
				if(find!=P->localFunctions.end()){
					//TODO check to see if was just name?
					countFunctions++;
				}
				for(const auto& a : P->children){
					Q.push(a);
				}
			}
			if(countFunctions==1){
				return SF->callFunction(r, id, v, instance);
			} else {
				//TODO consider evaluating args here?
				auto ENTRY = r.builder.GetInsertBlock();
				auto F = ENTRY->getParent();
				auto DEF = r.CreateBlock("_def", ENTRY);
				auto IV = instance->getValue(r, id);
				auto SWITCH = r.builder.CreateSwitch(r.builder.CreateLoad(r.builder.CreateConstGEP2_32(IV, 0, 1)), DEF, countClasses-1);

				auto retType = SF->getSingleProto()->returnType;
				bool isVoid = retType->classType==CLASS_VOID;

				auto DEST = r.CreateBlockD("_dest",F);
				r.builder.SetInsertPoint(DEST);
				llvm::PHINode* PHI;
				if(!isVoid) PHI = r.builder.CreatePHI(retType->type, countFunctions);

				r.builder.SetInsertPoint(DEF);
				auto dat = SF->callFunction(r, id, v, instance);
				if(!isVoid)
					PHI->addIncoming(dat->castToV(r, retType, id), r.builder.GetInsertBlock());
				r.builder.CreateBr(DEST);

				std::stack<std::tuple<UserClass* /* One to do */,const UserClass* /*To cast to*/,llvm::BasicBlock* /* To land*/> > Q;
				for(const auto& a : children){
					Q.push(std::tuple<UserClass* /* One to do */,const UserClass* /*To cast to*/,llvm::BasicBlock* /* To land*/>(a, this, DEF));
				}

				while(Q.size()!=0){
					auto M = Q.top();
					auto P = std::get<0>(M);
					Q.pop();
					const UserClass* TOCAST;
					llvm::BasicBlock* TOBREAK;
					auto find = P->localFunctions.find(s);
					if(find!=P->localFunctions.end()){
						TOCAST = P;
						TOBREAK = r.CreateBlockD("_dest",F);
						r.builder.SetInsertPoint(TOBREAK);
						if(!t_args.inUse)
							SF = find->second->getBestFit(id, v, true);
						else
							SF = find->second->getBestFit(id, t_args.eval(r, id), true);
						auto dat = SF->callFunction(r, id, v, instance);
						if(!isVoid)
							PHI->addIncoming(dat->castToV(r, retType, id), r.builder.GetInsertBlock());
						r.builder.CreateBr(DEST);
					} else {
						TOCAST = std::get<1>(M);
						TOBREAK = std::get<2>(M);
					}
					SWITCH->addCase(TOCAST->getValue(r, id),TOBREAK);
					for(const auto& a : P->children){
						Q.push(std::tuple<UserClass* /* One to do */,const UserClass* /*To cast to*/,llvm::BasicBlock* /* To land*/>(a, this, DEF));
					}
				}
				r.builder.SetInsertPoint(DEST);
				if(isVoid) return &VOID_DATA;
				else return new ConstantData(PHI, retType);
			}
		}
	}
	inline const AbstractClass* getLocalFunctionReturnType(PositionID id, String s, const T_ARGS& t_args, const std::vector<const AbstractClass*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) {
				if(!t_args.inUse)
					return find->second->getBestFit(id, v, true)->getSingleProto()->returnType;
				else
					return find->second->getBestFit(id, t_args.eval(rdata, id), true)->getSingleProto()->returnType;
			}
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	inline const AbstractClass* getLocalFunctionReturnType(PositionID id, String s, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) {
				if(!t_args.inUse)
					return find->second->getBestFit(id, v, true)->getSingleProto()->returnType;
				else
					return find->second->getBestFit(id, t_args.eval(rdata, id), true)->getSingleProto()->returnType;
			}
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}
	/*
	inline SingleFunction* getLocalFunction(PositionID id, String s, const T_ARGS& t_args, const std::vector<const Evaluatable*>& v) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()){
				if(!t_args.inUse)
					return find->second->getBestFit(id, v, true);
				else
					return find->second->getBestFit(id, t_args.eval(getRData(), id), true);
			}
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		return nullptr;
		//exit(1);
	}
	inline SingleFunction* getLocalFunction(PositionID id, String s, const T_ARGS& t_args, const std::vector<const AbstractClass*>& v) const{

		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) {
				if(!t_args.inUse)
					return find->second->getBestFit(id, v, true);
				else
					return find->second->getBestFit(id, t_args.eval(getRData(), id), true);
			}
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Could not find local method '"+s+"' in class '"+getName()+"'");
		exit(1);
	}*/

	inline llvm::Value* generateData(RData& r, PositionID id) const;
	inline bool hasLocalFunction(String s) const{
		auto tmp = this;
		do{
			auto find = tmp->localFunctions.find(s);
			if(find!=tmp->localFunctions.end()) return true;
			tmp = (const UserClass*)(tmp->superClass);
		}while(tmp);
		return false;
	}
	void finalize(PositionID id){
		assert(!final);
		final = true;
		if(superClass){
			UserClass* uc = (UserClass*)superClass;
			if(!uc->final) id.compilerError("Superclass is not finalized");
			start = uc->start+uc->localVars.size();
		} else start = 0;
		llvm::StructType* structType = (layout==POINTER_LAYOUT)?(
			(llvm::StructType*)(((llvm::PointerType*)type)->getArrayElementType())):
		((llvm::StructType*)type);
		int counter = start+localVars.size();
		llvm::SmallVector<llvm::Type*,0> types(counter);
		counter--;
		UserClass* tmp = this;
		do{
			const auto at=tmp->localVars.size();
			if(at >0){
			for(unsigned int i=at-1; ; i--){
				assert(counter>=0);
				assert((unsigned)counter<types.size());
				types[counter] = tmp->localVars[i]->type;
				counter--;
				if(i==0) break;
			}
			}
			tmp = (UserClass*) tmp->superClass;
		}while(tmp);
		assert(counter==-1);
		for(unsigned i=0; i<types.size();i++){
			assert(types[i]);
			//types[i]->dump();
			//cerr << endl << flush;
		}
		structType->setBody(types,false);
	}
	void addLocalVariable(PositionID id, String s, const AbstractClass* const ac){
		assert(ac);
		if(ac->layout==LITERAL_LAYOUT){
			id.error("Cannot store literal data as class variable");
			return;
		}
		if(ac->classType==CLASS_REF){
			id.error("Cannot store reference as class variable");
			return;
		}
		if(ac->classType==CLASS_LAZY){
			id.error("Cannot store lazy data as class variable");
			return;
		}
		auto tmp=this;
		do{
			if(tmp->localMap.find(s)!=tmp->localMap.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a variable in '"+tmp->getName()+"'");
				return;
			}
			if(tmp->localFunctions.find(s)!=tmp->localFunctions.end()){
				id.error("Cannot create local variable '"+s+"' in class '"+getName()+"', it is already defined as a function in '"+tmp->getName()+"'");
				return;
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		//if(staticVariables.exists(s)){}
		localMap[s]=localVars.size();
		localVars.push_back(ac);
	}
	bool hasLocalData(String s) const override final{
		if(!final) PositionID(0,0,"#user").compilerError("Cannot hasLocalData() on unfinalized type");
		auto tmp=this;
		do{
			auto fd = tmp->localMap.find(s);
			if(fd!=tmp->localMap.end()){
				return true;
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		return false;
	}
	const AbstractClass* getLocalReturnClass(PositionID id, String s) const override final{
		if(!final) id.compilerError("Cannot getLocalReturnClass() on unfinalized type");
		auto tmp=this;
			do{
				auto fd = tmp->localMap.find(s);
				if(fd!=tmp->localMap.end()){
					return tmp->localVars[fd->second];
				}
				tmp = (UserClass*)(tmp->superClass);
			}while(tmp);
		illegalLocal(id,s);
		exit(1);
	}

	const Data* getLocalData(RData& r, PositionID id, String s, const Data* instance) const override final;
	/*std::pair<AbstractClass*,unsigned int> getLocalVariable(PositionID id, String s) override final{
		if(!final) id.compilerError("Cannot getLocalVariable() on unfinalized type");
		auto tmp=this;
		do{
			auto fd = tmp->localMap.find(s);
			if(fd!=tmp->localMap.end()){
				return std::pair<AbstractClass*,unsigned int>(tmp->localVars[fd->second], tmp->start+fd->second);
			}
			tmp = (UserClass*)(tmp->superClass);
		}while(tmp);
		id.error("Cannot find local variable '"+s+"' in class '"+getName()+"'");
		return std::pair<AbstractClass*,unsigned int>(this,0);
	}*/
	bool noopCast(const AbstractClass* const toCast) const override{
		return hasCast(toCast);
		//todo decide if it is no-op class to
//		return toCast->classType==CLASS_BOOL;
	}
	bool hasCast(const AbstractClass* const toCast) const override{
		assert(toCast);
		if(toCast->classType==CLASS_VOID) return true;
		if(layout==POINTER_LAYOUT && toCast->classType==CLASS_CPOINTER) return true;
		if(toCast->classType!=CLASS_USER) return false;
		if(this==toCast) return true;
		if(layout==PRIMITIVE_LAYOUT || toCast->layout==PRIMITIVE_LAYOUT) return false;
		UserClass* tmp = (UserClass*)superClass;
		while(tmp!=nullptr){
			if(tmp==toCast) return true;
			else tmp=(UserClass*)tmp->superClass;
		}
		return false;
	}
	/**
	 * Will error with id if this.hasCast(toCast)==false
	 */
	llvm::Value* castTo(const AbstractClass* const toCast, RData& r, PositionID id, llvm::Value* valueToCast) const override{
		if(this==toCast) return valueToCast;
		if(toCast->classType==CLASS_CPOINTER && layout==POINTER_LAYOUT)
			return r.pointerCast(valueToCast);
		if(toCast->classType!=CLASS_USER){
			id.error("Cannot promote class '"+getName()+"' to "+toCast->getName());
			exit(1);
		}
		if(layout==PRIMITIVE_LAYOUT || toCast->layout==PRIMITIVE_LAYOUT){
			id.error("Cannot promote user-defined primitive types");
			exit(1);
		}
		UserClass* tmp = (UserClass*)superClass;
		assert(toCast->type->isPointerTy());
		while(tmp!=nullptr){
			if(tmp==toCast) return r.pointerCast(valueToCast, (llvm::PointerType*) toCast->type);
			else tmp=(UserClass*)tmp->superClass;
		}
		id.error("Cannot promote class '"+getName()+"' to "+toCast->getName());
		exit(1);
	}
	int compare(const AbstractClass* const a, const AbstractClass* const b) const{
		assert(hasCast(a));
		assert(hasCast(b));
		if(a->classType==CLASS_VOID && b->classType==CLASS_VOID) return 0;
		else if(a->classType==CLASS_VOID) return 1;
		else if(b->classType==CLASS_VOID) return -1;

		if(a->classType==CLASS_CPOINTER && b->classType==CLASS_CPOINTER) return 0;
		else if(a->classType==CLASS_CPOINTER) return 1;
		else if(b->classType==CLASS_CPOINTER) return -1;

		const UserClass* tmp = this;
		do{
			if(tmp==a){
				return (tmp==b)?(0):(-1);
			} else if(tmp==b) return 1;
			tmp = (UserClass*) ( tmp->superClass);
		}while(tmp!=nullptr);
		return 0;
	}
};

UserClass objectClass(&LANG_M,"object",nullptr,POINTER_LAYOUT,false,true);

#endif /* USERCLASS_HPP_ */
