/*
 * crossinfo.hpp
 *
 *  Created on: Sep 11, 2013
 *      Author: wmoses
 */

#ifndef CROSSINFO_HPP_
#define CROSSINFO_HPP_

#define INT_LENGTH 8
#define CHAR_LENGTH 1
#define C_INT_LENGTH sizeof(int)
#define C_LONG_LENGTH sizeof(long)
#define C_LONGLONG_LENGTH sizeof(long long)
#define C_CHAR_LENGTH sizeof(char)
#define C_POINTER_LENGTH sizeof(void*)

#include "settings.hpp"

auto C_CHARTYPE = IntegerType::get(getGlobalContext(), 8*C_CHAR_LENGTH);
auto C_INTTYPE = IntegerType::get(getGlobalContext(), 8*C_INT_LENGTH);
auto C_LONGTYPE = IntegerType::get(getGlobalContext(), 8*C_LONG_LENGTH);
auto C_LONG_LONGTYPE = IntegerType::get(getGlobalContext(), 8*C_LONGLONG_LENGTH);
auto C_STRINGTYPE = PointerType::get(C_CHARTYPE, 0);

auto VOIDTYPE = Type::getVoidTy (getGlobalContext());
auto BOOLTYPE = IntegerType::get(getGlobalContext(), 1);
auto INTTYPE = IntegerType::get(getGlobalContext(), 8*INT_LENGTH);
//auto LONGTYPE = IntegerType::get(getGlobalContext(), 8*INT_LENGTH);
auto DOUBLETYPE = Type::getDoubleTy(getGlobalContext());
auto CHARTYPE = IntegerType::get(getGlobalContext(), 8*CHAR_LENGTH);
auto COMPLEXTYPE = VectorType::get(DOUBLETYPE,2);

ConstantInt* getInt(uint64_t t){
	return ConstantInt::get(INTTYPE,t,true);
}
auto SLICETYPE = StructType::get(INTTYPE, INTTYPE, INTTYPE, NULL);
//std::vector<Type*> __str_struct = {INTTYPE, CHARPOINTER};
//auto STRINGTYPE = StructType::get(C_STRINGTYPE,INTTYPE,NULL);
//auto STRINGTYPE = StructType::create(__str_struct);

#endif /* CROSSINFO_HPP_ */
