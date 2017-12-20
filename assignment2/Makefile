CXX = g++
OPT = -fopenmp -O3
CXXFLAGS = -g -Wall $(OPT)

SRCDIR = src
INCLUDEDIR = include
OBJDIR = obj
BINDIR = bin

INCLUDE += -I$(INCLUDEDIR)
SRC = $(shell find $(SRCDIR) -iname "*.cpp" -o -iname "*.cc")
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC))
PROG = $(BINDIR)/rt

MOVIE = movie.mp4
MOVIE_FPS = 24
# Seconds
MOVIE_TIME = 3
FRAMEDIR = frames
SCENES = $(shell for i in {0..$(shell expr $(MOVIE_FPS) '*' $(MOVIE_TIME) - 1)}; do echo "frames/$$i.scn "; done)
FRAMES = $(patsubst %.scn, %.png, $(SCENES))
.PRECIOUS: $(FRAMEDIR)/%.scn

all: $(PROG)

$(MOVIE): $(FRAMES)
	@ffmpeg -nostats -loglevel 0 -y -framerate $(MOVIE_FPS) -i frames/%d.png $@
	@rm -rf $(FRAMEDIR)

$(FRAMEDIR)/%.png: $(FRAMEDIR)/%.scn
	@$(PROG) $<

$(FRAMES): $(PROG) | $(FRAMEDIR)

$(FRAMEDIR)/%.scn: make_scene.py
	@./make_scene.py $* $(MOVIE_FPS) $(MOVIE_TIME)

# Automatic Dependencies
# From http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d


$(PROG): $(OBJ) | $(BINDIR)
	$(CXX) $(CXXFLAGS) $+ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPDIR)/%.d | $(OBJDIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d : ;
.PRECIOUS : $(DEPDIR)/%.d

-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRC)))

$(OBJDIR) $(BINDIR) $(FRAMEDIR):
	@mkdir -p $@

.PHONY : clean format

format:
	clang-format -i -style=Google $(SRC) $(shell find $(INCLUDEDIR) -maxdepth 1 -iname "*.h" -o -iname "*.hpp")

clean:
	@rm -rf $(BINDIR) $(OBJDIR) $(FRAMEDIR)
