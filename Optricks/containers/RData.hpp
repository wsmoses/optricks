/*
 * RData.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef RDATA_HPP_
#define RDATA_HPP_

#include "settings.hpp"

DATA DATA::toLocation(RData& m){
	if(type==R_LOC) return *this;
	if(type!=R_CONST){
		cerr << "Cannot make non-value / non-loc to a location" << type << endl << flush;
		exit(1);
	}
	assert(data.constant && "Cannot get location of NULL");
	Type* t = data.constant->getType();
	AllocaInst* loc = m.builder.CreateAlloca(t,0);
	assert(loc);
	if(t->isAggregateType() || t->isArrayTy() || t->isVectorTy() || t->isStructTy()){
		m.builder.CreateStore(data.constant,loc);
		return getLocation(new StandardLocation(loc),getReturnType(m));
	} else
		return getLocation(new LazyLocation(m,loc,m.builder.GetInsertBlock(),data.constant),getReturnType(m));
}
Value* DATA::getValue(RData& r,PositionID id) const{
	assert(data.pointer!=NULL);
	if(type==R_CONST) return data.constant;
	else if(type==R_FUNC) return data.function;
	else if(type==R_LOC){
		Value* v = data.location->getValue(r,id);
		return v;
	}
	else{
		cerr << "Cannot get Value of DataType " << type << endl << flush;
		exit(1);
	}
};
void DATA::setValue(RData& r, Value* v){
	assert(data.pointer!=NULL && "Cannot set NULL DATA");
	if(type==R_LOC) data.location->setValue(v,r);
	else{
		cerr << "Cannot set Value of DataType " << type << endl << flush;
		exit(1);
	}
};

#include "ClassProto.hpp"
BasicBlock* RData::getBlock(String name, JumpType jump, BasicBlock* bb, DATA val, PositionID id, std::pair<BasicBlock*,BasicBlock*> resume){
	if(name==""){
		if(jump==RETURN || jump==YIELD){
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == FUNC || jumps[i]->toJump==GENERATOR){
					if(jumps[i]->returnType==voidClass){
						if(val.getType()!=R_UNDEF && val.getReturnType(*this)!=voidClass) id.error("Cannot return something in function requiring void");
						jumps[i]->endings.push_back(std::pair<BasicBlock*,DATA>(bb, DATA::getNull()));
					}
					else jumps[i]->endings.push_back(std::pair<BasicBlock*,DATA>(bb, val.castTo(*this, jumps[i]->returnType, id)));
					jumps[i]->resumes.push_back(resume);
					return jumps[i]->end;
				}
				if(i <= 0){
					cerr << "Error could not find returning block" << endl << flush;
					return NULL;
				}
			}
		} else {
			for(int i = jumps.size()-1; ; i--){
				if(jumps[i]->toJump == LOOP){
					//jumps[i]->endings.push_back(std::pair<BasicBlock*,Value*>(bb,val));
					return (jump==BREAK)?(jumps[i]->end):(jumps[i]->start);
				}
				if(i <= 0){
					cerr << "Error could not find continue/break block" << endl << flush;
					return NULL;
				}
			}
		}
	} else {
		cerr << "Error not done yet2" << endl << flush;
		exit(1);
		return NULL;
	}
}

Function* o_malloc;
Function* o_calloc;
Function* o_free;
Function* o_realloc;
Function* o_memset;
Function* o_memcpy;
//Function* o_glutInit;
Function* o_strlen;

template<typename T> inline ArrayRef<T> aref(std::initializer_list<T> li){
	return ArrayRef<T>(li.begin(), li.end());
}

#define types(...) ArrayRef<Type*>(aref<Type*>({__VA_ARGS__}))
#define PT(A) PointerType::getUnqual(A)
#define FT(A,B,C) FunctionType::get(A,B,C)
#define FC(A,B,C) C = Function::Create(A,EXTERN_FUNC,#B,r.lmod); r.exec->addGlobalMapping(C, (void*)(&B));
#define FCR(A,B) Function::Create(A,LOCAL_FUNC,B,r.lmod)
void initializeBaseFunctions(RData& r){

	FC(FT(C_POINTERTYPE, types(C_INTTYPE), false),malloc,o_malloc);

	/** %1 number of elements, each %2 long */

	FC(FT(C_POINTERTYPE, types(SIZETYPE, SIZETYPE), false),calloc,o_calloc);

	FC(FT(VOIDTYPE, types(C_POINTERTYPE), false),free,o_free);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,SIZETYPE), false),realloc,o_realloc);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,C_INTTYPE,SIZETYPE), false),memset,o_memset);

	FC(FT(C_POINTERTYPE, types(C_POINTERTYPE,C_POINTERTYPE,SIZETYPE), false),memcpy,o_memcpy);

	FC(FT(INTTYPE,types(C_STRINGTYPE),false),strlen,o_strlen);
	/*
	{
		Function* FC(FT(VOIDTYPE, types(PointerType::getUnqual(C_INTTYPE),PointerType::getUnqual(C_STRINGTYPE)), false), glutInit,o_glutInitI);
		o_glutInit = FCR(FT(VOIDTYPE, types(), false),"#glutInit");
		BasicBlock *Parent = r.builder.GetInsertBlock();
		BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", o_glutInit);
		r.builder.SetInsertPoint(BB);
		Value* v = r.builder.CreateAlloca(C_INTTYPE,getCInt(0));
		r.builder.CreateStore(getCInt(0),v);
		r.builder.CreateCall2(o_glutInitI, v, UndefValue::get(PointerType::getUnqual(C_STRINGTYPE)));
		r.builder.CreateRetVoid();
		if(Parent!=NULL) r.builder.SetInsertPoint(Parent);
	}*/
}
#undef FC
#undef FT
#undef PT
#undef types

#endif /* RDATA_HPP_ */
