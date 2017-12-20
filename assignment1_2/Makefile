CXX = g++
CXXFLAGS = -Wall
GL = -framework OpenGL
LIB = -lSDL2 -lSDL2main

OS := $(shell uname)
ifeq ($(OS), Linux)
	GL = -ldl -lGL
endif

all: blank cube3d cubelit cubelit1vbo triangle model model_orig

blank: BlankScreenSDL.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

cube3d: Cube3D.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

cubelit: CubeLit.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

cubelit1vbo: CubeLit1VBO.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

triangle: TriangleColored.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

model: ModelLoad.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)

model_orig: ModelLoad2.cpp
	$(CXX) $(CXXFLAGS) $+ -x c glad/glad.c -o $@ $(LIB) $(GL)


clean:
	@rm -rf blank cube3d cubelit cubelit1vbo triangle model model_orig *.dSYM
