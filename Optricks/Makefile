all:
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs core jit native` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -g -ggdb
