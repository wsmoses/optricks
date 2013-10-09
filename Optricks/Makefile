all: 
	g++ -std=c++11 main.cpp -L/usr/local/lib -o Optricks `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -g -ggdb

clang: 
	clang++ -std=c++11 main.cpp -L/usr/local/lib -o Optricks `llvm-config-3.4 --cppflags --libs all` `llvm-config-3.4 --ldflags` ${CPPFLAGS} ${LDFLAGS} -g -ggdb -static-libgcc -static

shared: 
	clang++ -std=c++11 main.cpp -L/usr/local/lib -o Optricks `llvm-config-3.4 --cppflags --libs all` `llvm-config-3.4 --ldflags` ${CPPFLAGS} ${LDFLAGS} -g -ggdb
	
release: 
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs all` `llvm-config --ldflags`	${CPPFLAGS} ${LDFLAGS} -O3 -static-libgcc -static

#func:
#	g++ -std=c++11 -c containers/basic_functions.cpp -o containers/basic_functions.o
#	ar ruv libbasic_functions.a containers/basic_functions.o
