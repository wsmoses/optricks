
#ifndef O_IO_HPP_
#define O_IO_HPP_
#include "O_Token.hpp"
#include "containers/all.hpp"


bool writeByte(FILE* f, byte a){
	return fputc(a, f)==EOF;
}

bool readByte(FILE* f, byte* a){
	*a = fgetc(f);
	return false;
}

bool peekByte(FILE* f, byte* a){
	*a = fgetc(f);
	return ungetc(*a,f)==EOF;
}

bool writeChar(FILE* f, char a){
	return fputc(a,f)==EOF;
}

bool readChar(FILE* f, char *a){
	*a = fgetc(f);
	return false;
}

bool returnChar(FILE* f, char a){
	return ungetc(a,f)==EOF;
}

bool peekChar(FILE* f, char* a){
	*a = fgetc(f);
	return ungetc(*a,f)==EOF;
}

bool returnString(FILE* f, String c){
	for(unsigned int i = c.length()-1; ; i--){
		if(returnChar(f, (char)c[i])) return true;
		if(i==0) break;
	}
	return false;
}

#endif /* O_IO_HPP_ */
