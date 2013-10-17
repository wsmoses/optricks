#ifndef __BASICFUNCH
#define __BASICFUNCH

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
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
#ifndef PRId64
		if(!b) printf("%lld", i);
		else printf("%lld\n",i);
#else
		if(!b) printf("%" PRId64 "", i);
		else printf("%" PRId64 "\n",i);

#endif
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
			if(i) printf("true");
			else printf("false");
		}
		else{
			if(i) puts("true");
			else puts("false");
			fflush(stdout);
		}
	}
}
#endif
