TARGET = opencsgexample

SRC_FILES = $(wildcard src/*.cpp) $(wildcard example/*.cpp) RenderTexture/RenderTexture.cpp

GCC = gcc
CXX = g++
CC = $(CXX)

# For an optimized version without debugging code
#  DEBUG_LEVEL = -O2 -DNDEBUG
DEBUG_LEVEL = -g -D_DEBUG

LD_FLAGS =
WARN_CCFLAGS = -W -Wall

CCFLAGS = $(DEBUG_LEVEL) $(LD_FLAGS) $(WARN_CCFLAGS) $(EXTRA_FLAGS)

EXTRA_INCLUDE =

INCLUDE = $(EXTRA_INCLUDE) -Iinclude -I. -Iglew/include -DGLEW_STATIC

LIBRARIES = -ltk -ltcl -lGL -lGLU -lglut -lm -lfreetype -lXmu -lz -lpng -lGLEW
LIBPATH = -Lglew/lib -L/usr/lib -L/usr/X11R6/lib 

LIB = $(LIBPATH) $(LIBRARIES)

O_FILES = $(SRC_FILES:%.cpp=%.o)

all:
	$(CXX) $(CCFLAGS) $(LIB) $(INCLUDE) $(SRC_FILES) -o $(TARGET)

clean:
	rm -f $(O_FILES) $(TARGET)

