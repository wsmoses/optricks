/*
 * DATA.hpp
 *
 *  Created on: Nov 21, 2013
 *      Author: wmoses
 */

#ifndef DATA_CPP_
#define DATA_CPP_

/*#include "DATA.hpp"
#include "../containers/ClassProto.hpp"
#include "../containers/Location.hpp"

ClassProto* DATA::getReturnType() const{
	if(type==R_GEN){
		assert(info.funcType!=NULL);
		return info.funcType->getGeneratorType();
	}
	if(type==R_FUNC) return functionClass;
	if(!(type==R_CONST || type==R_LOC || type==R_CLASS)){
		printf("You've run into a compiler error!\n");
		printf("Most likely this is because you tried to use a global variable in a function\n");
		printf("This is not yet supported but will be soon.\n");//TODO global variable in function
		//printf("Although you probably shouldn't be using globals");
	}
#ifndef NDEBUG
	if(!(type==R_CONST || type==R_LOC || type==R_CLASS)){
		cerr << "Error " << str<DataType>(type) << endl << flush;
		assert(0);
	}

#endif
//	if(type!=R_CONST && type!=R_LOC && type!=R_CLASS) id.error("Could not get returnType of "+str<DataType>(type));
	assert(info.classType !=NULL);
	return info.classType;
}

DATA DATA::getClass(ClassProto* c){
	return DATA(R_CLASS, c, classClass);
};

DATA DATA::castTo(RData& r, ClassProto* right, PositionID id) const{
#ifdef NDEBUG
	if(!(type==R_CONST || type==R_LOC || type==R_FUNC)){
		cerr << "Compiler Error: Illegal data type cast " << type << endl << flush;
		exit(1);
	}
#else
	assert(type==R_CONST || type==R_LOC || type==R_FUNC);
#endif
	assert(right!=NULL);
	ClassProto* left = getReturnType();
	if(left == right) return *this;
	if(((left->layoutType==POINTER_LAYOUT && right->layoutType==POINTER_LAYOUT) || (left->layoutType==PRIMITIVEPOINTER_LAYOUT && right->layoutType==PRIMITIVEPOINTER_LAYOUT)) &&
			left->hasSuper(right)){
		if(type==R_LOC) return DATA::getLocation(data.location, right);
		else if(type==R_CONST) return DATA::getConstant(data.constant, right);
		else assert(0 && "this type is invalid");
	}
	ouop* c = left->getCast(right,id);
	return c->apply(*this, r, id);
}

ClassProto* DATA::getMyClass() const{
	if(type!=R_CLASS && type!=R_GEN){
		PositionID(0,0,"<start.getMyClass>").error("Cannot getMyClass of non-class "+str<DataType>(type));
	}
	assert(data.pointer!=NULL);
	if(type==R_GEN){
		ClassProto* tmp = info.funcType->getGeneratorType();
		assert(tmp!=NULL);
		return tmp;
	}
	return data.classP;
}

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
		return getLocation(new StandardLocation(loc),getReturnType());
	} else
		return getLocation(new LazyLocation(m,loc,m.builder.GetInsertBlock(),data.constant),getReturnType());
}

Value* DATA::getValue(RData& r,PositionID id) const{
	assert(type!=R_UNDEF);
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
*/
#endif /* DATA_HPP_ */
