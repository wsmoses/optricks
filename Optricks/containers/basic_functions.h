#ifndef __BASICFUNCH
#define __BASICFUNCH

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
typedef struct StringStruct_s{
		char* data;
		int64_t length;
} StringStruct;
typedef struct complex_c{
		double real, imag;
} complex;
void prints(StringStruct i, bool b){
		if(!b) puts(i.data);
		else{
			printf("%s\n",i.data);
			fflush(stdout);
		}
	}
void printi(int64_t i, bool b){
#ifndef PRId64
		if(!b) printf("%lld", i);
		else{
			printf("%lld\n",i);
			fflush(stdout);
		}
#else
		if(!b) printf("%" PRId64 "", i);
		else{
			printf("%" PRId64 "\n",i);
			fflush(stdout);
		}

#endif
}
void printby(uint8_t i, bool b){
	if(!b) printf("%d",i);
	else{
		printf("%d\n",i);
		fflush(stdout);
	}
}
	void printd(double i, bool b){
		if(!b) printf("%f",i);
		else{
			printf("%f\n",i);
			fflush(stdout);
		}
	}
	void printc(complex i, bool b){
		if(!b) printf("%f + %f i",i.real,i.imag);
		else{
			printf("%f + %f i\n",i.real,i.imag);
			fflush(stdout);
		}
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
#endif
