
#ifndef O_IO_HPP_
#define O_IO_HPP_
#include "O_Token.hpp"
#include "containers/settings.hpp"


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


//TODO
bool writeDouble(FILE* f, double a){
	return true;
}

//TODO
bool readDouble(FILE* f, double* a){
	*a = 0;
	return true;
}

//TODO
bool writeLong(FILE* f, long int a){
	return true;
}

//TODO
bool readLong(FILE* f, long int* a){
	*a = 0;
	return true;
}

//TODO
bool writeInteger(FILE* f, int a){
	return true;
}

//TODO
bool readInteger(FILE* f, int* a){
	*a = 0;
	return true;
}

//TODO
bool writeUInteger(FILE* f, unsigned int a){
	return true;
}

//TODO
bool readUInteger(FILE* f, unsigned int* a){
	a = 0U;
	return true;
}

bool writeString(FILE* f, String a){
	size_t fin = a.length();
	if(writeUInteger(f, a.length())) return true;
	for(unsigned int i = 0; i<fin; i++) if(writeChar(f, (char)a[0])) return true;
	return false;
}

bool readString(FILE* f, String* a){
	unsigned int fin;
	if(readUInteger(f, &fin)) return true;
	a->resize(fin);
	char tempC;
	for(unsigned int i = 0; i<fin; i++){
		if(readChar(f, &tempC)) return true;
		(*a)[i] = tempC;
	}
	return false;
}

bool returnString(FILE* f, String c){
	for(unsigned int i = c.length()-1; i>=0; i--){
		if(returnChar(f, (char)c[i])) return true;
	}
	return false;
}

#endif /* O_IO_HPP_ */
