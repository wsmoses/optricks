all: func
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs 	all` `llvm-config --ldflags`	${CPPFLAGS} ${LDFLAGS} -g 	-ggdb -static-libgcc -static containers/basic_functions.o

release: func
	g++ -std=c++11 main.cpp -o Optricks `llvm-config --cppflags --libs all` `llvm-config --ldflags`	${CPPFLAGS} ${LDFLAGS} -O3 -static-libgcc -static containers/basic_functions.o

func:
	g++ -std=c++11 -c containers/basic_functions.cpp -o containers/basic_functions.o
	ar ruv libbasic_functions.a containers/basic_functions.o
