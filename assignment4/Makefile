CXX = g++
CXXFLAGS = -g -Wall -std=c++17

SRCDIR = src
INCLUDEDIR = include
OBJDIR = obj
BINDIR = bin

SRC = $(shell find $(SRCDIR) -iname "*.cpp")
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
PROG = $(BINDIR)/game

GLAD = $(INCLUDEDIR)/glad/glad.c
GL = -framework OpenGL
LIB = -lSDL2 -lSDL2main
INCLUDE += -I$(INCLUDEDIR)

OS := $(shell uname)
ifeq ($(OS), Linux)
	GL = -ldl -lGL
endif

# Automatic Dependencies
# From http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(PROG)

$(PROG): $(OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $+ -x c $(GLAD) -o $@ $(LIB) $(GL)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPDIR)/%.d | $(OBJDIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d : ;
.PRECIOUS : $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC)))

$(OBJDIR) $(BINDIR):
	mkdir -p $@

.PHONY : clean format

format:
	clang-format -i -style=Google $(SRC) $(INCLUDEDIR)/*.h

clean:
	@rm -rf $(BINDIR) $(OBJDIR)
