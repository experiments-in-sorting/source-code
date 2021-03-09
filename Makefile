# Makefile for MergeSort

# *****************************************************
# Variables to control Makefile operation

CXX = g++
RM = rm -f -r

# Directory this example is contained in
MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
UNAME    := $(shell uname)

# Dependencies
_DEPS = \

# Main source file
TARGET = insertion_sort

# Object files
ODIR = obj
_OBJ = $(TARGET).o
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))

# To create obj directory
dummy_build_folder := $(shell mkdir -p $(ODIR))

# Flags
NOWARN = -Wno-unused-parameter -Wno-unused-function -Wno-narrowing \
			-Wno-sizeof-array-argument -Wno-sign-compare -Wno-unused-variable

ifeq ($(UNAME), Linux)
GL_FLAGS := -lGLU  -lGL
BREW :=
endif
ifeq ($(UNAME), CYGWIN_NT-10.0)
GL_FLAGS := -lGLU  -lGL
BREW :=
endif
ifeq ($(UNAME), Darwin)
GL_FLAGS := -framework OpenGL 
BREW := -lomp -I"$(brew --prefix libomp)/include" 
endif

CXXFLAGS = -O3 -g3 -ggdb3 \
	-Wall -Wextra \
	-D__GXX_EXPERIMENTAL_CXX0X__ \
	-I/usr/local/include/Cellar/glfw3/3.3/include/ \
	-I/usr/local/include/TSGL \
	-I/usr/local/include \
	-I/usr/local/include/freetype2 \
	-I/usr/local/include/freetype2/freetype \
	$(NOWARN)\
	-std=c++11\
	-Wc++11-compat\
	-I/opt/AMDAPP/include/ \
	-I/usr/include/c++/4.6/ \
	-I/usr/include/c++/4.6/x86_64-linux-gnu/ \
	-I/usr/lib/gcc/x86_64-linux-gnu/9/include/ \
	-I/usr/include \
	-I/usr/include/freetype2 \
	-I/usr/include/freetype2/freetype  \

LFLAGS = -g -ltsgl -ltsal -lfreetype -lGLEW -lglfw $(GL_FLAGS) -fopenmp  \
			$(BREW) -L/usr/local/lib \


# ****************************************************
# Targets needed to bring the executable up to date

all: $(TARGET)

$(ODIR)/%.o: %.cpp $(_DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LFLAGS)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	$(RM) $(ODIR)/*.o $(ODIR) $(TARGET)
	@echo ""
	@tput setaf 5;
	@echo "*************** All output files removed from $(DIR)! ***************"
	@tput sgr0;
	@echo ""