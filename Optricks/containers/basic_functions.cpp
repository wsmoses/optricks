#ifndef __BASICFUNC
#define __BASICFUNC
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdint.h>
#include "basic_functions.hpp"
void prints(StringStruct i, bool b){
	std::cout << "String length is " << i.length << std::endl << std::flush;
	std::cout << std::string(i.data, i.length);
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
void printi(uint64_t i, bool b){
	std::cout << i;
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
void printd(double i, bool b){
	std::cout << i;
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
void printb(bool i, bool b){
	if(i) std::cout << "true";
	else std::cout << "false";
	if(b) std::cout << std::endl;
	std::cout << std::flush;
}
#endif
