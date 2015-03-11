GCC = g++
CLANG = clang++ --analyze

CC = $(GCC)

FLAGS = -lmpfr -lgmp ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

LLVM = `~/instllvm/llvm/bin/llvm-config --cppflags --libs all --ldflags` -lm -lz -ldl -lpthread -ltinfo

UNAME := $(shell uname)

OPENGL = -DUSE_OPENGL

SDL = -DUSE_SDL -lSDL2_mixer `sdl2-config --cflags --libs`

DEBUG = -DWITH_ASSERTS -g -ggdb -fno-rtti
RELEASE = -O3 -lpthread -fno-rtti
#RELEASE = -O3 -static-libgcc -DNDEBUG -static

# MSYS or CYGWIN or MINGW
ifneq (,$(findstring NT, $(UNAME)))
	#FLAGS += -static -lglibc -Wl,-Bdynamic
	LLVM += -limagehlp
	SDL += -lWinmm
	OPENGL += -lglut -lglu32 -lopengl32
else ifeq ($(UNAME), Darwin)
	OPENGL += -I/opt/X11/include -framework OpenGL -framework GLUT -framework Cocoa
	GCC = g++-4.9
else
	OPENGL += -lGLU -lglut
endif

STD = -std=c++11

all: 
	@echo "Enabled  OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(OPENGL) $(SDL) $(DEBUG)

no-graphics:
	@echo "Disabled OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(SDL) $(DEBUG)

no-sdl:	
	@echo "Enabled  OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(OPENGL) $(DEBUG)

bare:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(DEBUG)

bare-wall:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(DEBUG) -Wall -fdiagnostics-color=always

release-all: 
release:
	@echo "Enabled  OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) $(OPENGL) $(SDL)

release-no-graphics:
	@echo "Disabled OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) $(SDL)

release-no-sdl:	
	@echo "Enabled  OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) $(OPENGL)

release-bare:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE)

profile-bare:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) -pg

profile-all:
profile:
	@echo "Enabled  OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) $(OPENGL) $(SDL) -pg
