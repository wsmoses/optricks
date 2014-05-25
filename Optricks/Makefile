GCC = g++
CLANG = clang++ --analyze

CC = $(GCC)

LIB = `llvm-config --cppflags --libs all` -lclang `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -I/mingw64/include

DEBUG = -g -ggdb

STD = -std=c++11

all: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) $(DEBUG)

all-win: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) $(DEBUG) -limagehlp

clang-win: 
	$(CLANG) $(STD) Main.cpp -o Optricks $(LIB) $(DEBUG) -limagehlp

ddebug: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) $(DEBUG) -DNDEBUG

fast:
	$(CC) $(STD) Main.cpp -o Optricks $(LIB)

release: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) -O3 -static-libgcc -static -DNDEBUG -fno-rtti

release-win: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) -O3 -static-libgcc -static -DNDEBUG -fno-rtti -limagehlp

func:
	gcc -c containers/basic_functions.hpp -o containers/basic_functions.o
#	ar ruv libbasic_functions.a containers/basic_functions.o
