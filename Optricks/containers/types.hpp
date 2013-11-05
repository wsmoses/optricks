/*
 * crossinfo.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef CROSSINFO_HPP_
#define CROSSINFO_HPP_
#include <inttypes.h>
#include <stdint.h>
#define INT_LENGTH 8
#define CHAR_LENGTH 1
#define C_INT_LENGTH sizeof(int)
#define C_LONG_LENGTH sizeof(long)
#define C_LONGLONG_LENGTH sizeof(long long)
#define C_CHAR_LENGTH sizeof(char)
#define C_POINTER_LENGTH sizeof(void*)

#if INT_LENGTH == 8
#define opt_int int64_t
#elif INT_LENGTH==4
#define opt_int int32_t
#endif
/*
#if INT_LENGTH == sizeof(signed int)
#define opt_int (signed int)
#elif INT_LENGTH == sizeof(signed long)
#define opt_int (signed long)
#elif INT_LENGTH == sizeof(signed long long)
#define opt_int (signed long long)
#else
# error "Cannot find c type that matches length of optricks integer"
#endif
*/
#include "settings.hpp"

auto SIZETYPE = IntegerType::get(getGlobalContext(), 8*sizeof(size_t));
auto C_CHARTYPE = IntegerType::get(getGlobalContext(), 8*C_CHAR_LENGTH);
auto C_INTTYPE = IntegerType::get(getGlobalContext(), 8*C_INT_LENGTH);
auto C_LONGTYPE = IntegerType::get(getGlobalContext(), 8*C_LONG_LENGTH);
auto C_LONG_LONGTYPE = IntegerType::get(getGlobalContext(), 8*C_LONGLONG_LENGTH);
auto C_STRINGTYPE = PointerType::get(C_CHARTYPE, 0);
auto C_POINTERTYPE = PointerType::get(C_CHARTYPE,0);//IntegerType::get(getGlobalContext(), 8*C_POINTER_LENGTH);

auto VOIDTYPE = Type::getVoidTy (getGlobalContext());
auto BOOLTYPE = IntegerType::get(getGlobalContext(), 1);
auto INT32TYPE = IntegerType::get(getGlobalContext(), 32);
auto UINT32TYPE = IntegerType::get(getGlobalContext(), 32);
auto INTTYPE = IntegerType::get(getGlobalContext(), 8*INT_LENGTH);
auto FUNCTIONTYPE = PointerType::getUnqual(FunctionType::get(Type::getVoidTy(getGlobalContext()), ArrayRef<Type*>(),false));
//auto LONGTYPE = IntegerType::get(getGlobalContext(), 8*INT_LENGTH);
auto DOUBLETYPE = Type::getDoubleTy(getGlobalContext());
auto CHARTYPE = IntegerType::get(getGlobalContext(), 8*CHAR_LENGTH);
auto BYTETYPE = IntegerType::get(getGlobalContext(), 8);
auto COMPLEXTYPE = VectorType::get(DOUBLETYPE,2);
auto RATIONALTYPE = VectorType::get(INTTYPE,2);


ConstantInt* getUInt32(uint32_t t){
	return ConstantInt::get(INT32TYPE,t,false);
}
ConstantInt* getInt32(int32_t t){
	return ConstantInt::get(INT32TYPE,t,true);
}
ConstantInt* getBool(bool b){
	return ConstantInt::get(BOOLTYPE, b, false);
}
ConstantFP* getDouble(APFloat t){
	return ConstantFP::get(getGlobalContext(), t);
}
ConstantFP* getDouble(double t){
	return ConstantFP::get(getGlobalContext(), APFloat(t));
//	return ConstantFP::get(DOUBLETYPE, t);
}
ConstantInt* getByte(uint8_t t){
	return ConstantInt::get(BYTETYPE,t,false);
}

ConstantInt* getSize(uint64_t t){
	return ConstantInt::get(SIZETYPE,t,false);
}
ConstantInt* getCInt(int64_t t){
	return ConstantInt::get(C_INTTYPE,t,true);
}
ConstantInt* getInt(int64_t t){
	return ConstantInt::get(INTTYPE,t,true);
}
auto SLICETYPE = StructType::get(INTTYPE, INTTYPE, INTTYPE, NULL);
//std::vector<Type*> __str_struct = {INTTYPE, CHARPOINTER};
//auto STRINGTYPE = StructType::get(C_STRINGTYPE,INTTYPE,NULL);
//auto STRINGTYPE = StructType::create(__str_struct);

#endif /* CROSSINFO_HPP_ */
