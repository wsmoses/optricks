#ifndef __BASICFUNCH
#define __BASICFUNCH

struct StringStruct{
		int length;
		char* data;
};
void prints(StringStruct s, bool b);
void printi(uint64_t i, bool b);
void printd(double i, bool b);
void printb(bool i, bool b);
#endif
