GCC = g++
CLANG = clang++ --analyze

CC = $(GCC)

LIB = `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -lSDL2_mixer -lSDL2_image `sdl2-config --cflags --libs`
#LIB = `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -Wl,--whole-archive -lSDL2_mixer -Wl,--no-whole-archive `sdl2-config --cflags --static-libs` -L/usr/local/cross-tools/i386-mingw32/lib

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
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) -O3 -static-libgcc -static -DNDEBUG -limagehlp

release: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) -O3 -static-libgcc -static -DNDEBUG -fno-rtti

release-win: 
	$(CC) $(STD) Main.cpp -o Optricks $(LIB) -O3 -static-libgcc -static -DNDEBUG -fno-rtti -limagehlp

func:
	gcc -c containers/basic_functions.hpp -o containers/basic_functions.o
#	ar ruv libbasic_functions.a containers/basic_functions.o
