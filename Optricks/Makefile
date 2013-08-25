all:
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs 	core jit native` `llvm-config --ldflags`	${CPPFLAGS} ${LDFLAGS} -g 	-ggdb -static-libgcc -static

release:
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs 	core jit native` `llvm-config --ldflags`	${CPPFLAGS} ${LDFLAGS} -O3 -static-libgcc -static
