GCC = g++
CLANG = clang++ --analyze

CC = $(GCC)

FLAGS = -lmpfr -lgmp ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

LLVM = `llvm-config --cppflags --libs all` `llvm-config --ldflags` -lm -lz

UNAME := $(shell uname)

OPENGL = -DUSE_OPENGL

SDL = -DUSE_SDL -lSDL2_mixer -lSDL2_image `sdl2-config --cflags --libs`

DEBUG = -DWITH_ASSERTS -g -ggdb
RELEASE = -O3 -static-libgcc -DNDEBUG
#RELEASE = -O3 -static-libgcc -DNDEBUG -static

# MSYS or CYGWIN or MINGW
ifneq (,$(findstring NT, $(UNAME)))
#	FLAGS += -ws2_32
	LLVM += -limagehlp
	SDL += -lWinmm
	OPENGL += -lopengl32 -lglu32 -lglut
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
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(OPENGL) $(SDL) $(RELEASE) -static-libgcc -static-libstdc++

release-no-graphics:
	@echo "Disabled OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(SDL) $(RELEASE) -static-libgcc -static-libstdc++

release-no-sdl:	
	@echo "Enabled  OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(OPENGL) $(RELEASE) -static-libgcc -static-libstdc++

release-bare:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) -static -static-libgcc -static-libstdc++

profile-bare:
	@echo "Disabled OpenGL"
	@echo "Disabled SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(RELEASE) -static -static-libgcc -static-libstdc++ -pg

profile-all:
profile:
	@echo "Enabled  OpenGL"
	@echo "Enabled  SDL"
	$(CC) $(STD) Main.cpp -o Optricks $(FLAGS) $(LLVM) $(OPENGL) $(SDL) $(RELEASE) -static-libgcc -static-libstdc++ -pg
