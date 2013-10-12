#ifndef __BASICFUNCH
#define __BASICFUNCH

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
extern "C" {
struct StringStruct{
		char* data;
		int64_t length;
};
struct complex{
		double real, imag;
};
void prints(StringStruct i, bool b){
		if(!b) puts(i.data);
		else printf("%s\n",i.data);
	}
	void printi(int64_t i, bool b){
		if(!b) printf("%" PRId64 "", i);
		else printf("%" PRId64 "\n",i);
	}
	void printd(double i, bool b){
		if(!b) printf("%f",i);
		else printf("%f\n",i);
	}
	void printc(complex i, bool b){
		if(!b) printf("%f + %f i",i.real,i.imag);
		else printf("%f + %f i\n",i.real,i.imag);
	}
	void printb(bool i, bool b){
		if(!b){
			if(i) puts("true");
			else puts("false");
		}
		else{
			if(i) puts("true\n");
			else puts("false\n");
			fflush(stdout);
		}
	}
}
#endif
