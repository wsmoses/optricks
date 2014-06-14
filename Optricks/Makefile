GCC = g++
CLANG = clang++ --analyze

CC = $(GCC)

FLAGS = -lmpfr -lgmp ${CPPFLAGS} ${CFLAGS} ${LDFLAGS}

LLVM = `llvm-config --cppflags --libs all` `llvm-config --ldflags` -lm -lz

UNAME := $(shell uname)

OPENGL = -DUSE_OPENGL

SDL = -DUSE_SDL -lSDL2_mixer -lSDL2_image `sdl2-config --cflags --libs`

RELEASE = -O3 -static-libgcc -DNDEBUG
#RELEASE = -O3 -static-libgcc -DNDEBUG -static

ifneq (,$(findstring MINGW, $(UNAME)))
	LLVM += -limagehlp
	SDL += -lWinmm
	OPENGL += -lopengl32 -lglu32 -lglut
else ifeq ($(UNAME), Darwin)
	OPENGL += -I/opt/X11/include -framework OpenGL -framework GLUT -framework Cocoa
else
	OPENGL += -lGLU -lglut
endif

#LIB = `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -lSDL2_mixer -lSDL2_image `sdl2-config --cflags --libs`
#LIB = -DUSE_OPENGL `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -Wl,--whole-archive -lSDL2_mixer -Wl,--no-whole-archive `sdl2-config --cflags --static-libs`
#LIB = `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -Wl,--whole-archive -lSDL2_mixer -Wl,--no-whole-archive `sdl2-config --cflags --static-libs`
#LIB = -Wl,-Bstatic `llvm-config --cppflags --libs all` `llvm-config --ldflags` ${CPPFLAGS} ${LDFLAGS} -lmpfr -lgmp -lm -lz -Wl,-Bdynamic -Wl,--whole-archive -lSDL2_mixer -Wl,--no-whole-archive -Wl,-Bstatic `sdl2-config --cflags --static-libs`

LIB = $(FLAGS) $(LLVM) $(OPENGL) $(SDL)

DEBUG = -g -ggdb

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
