CXX = g++
SRC = Square.cpp -x c glad/glad.c

# Note: Installed SDL2 on Mac with `brew install sdl2`
LIB = -lSDL2 -lSDL2main
GL = -framework OpenGL
PROG = square
PLATFORM = mac

OS := $(shell uname)
ifeq ($(OS), Linux)
	GL = -ldl -lGL
	PLATFORM = linux
endif

all: square square-wide square-narrow

square: Square.cpp
	$(CXX) $(SRC) $(LIB) $(GL) -o $@

square-narrow: Square.cpp
	$(CXX) $(SRC) $(LIB) $(GL) -o $@ -DSCREEN_HEIGHT=800 -DSCREEN_WIDTH=400

square-wide: Square.cpp
	$(CXX) $(SRC) $(LIB) $(GL) -o $@ -DSCREEN_HEIGHT=400 -DSCREEN_WIDTH=800

.PHONY: clean binaries

clean:
	@rm -f square square-wide square-narrow square-*.zip

binaries: all
	@zip square-$(PLATFORM) square square-wide square-narrow
