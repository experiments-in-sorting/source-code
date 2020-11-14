CC:= g++
OBJS:= bubble_sort merge_sort quick_sort insertion_sort
CXXFLAGS:= -Wall -g -fopenmp
LDLIBS:= -ltsal

all: $(OBJS)

.PHONY: clean
clean:
	rm -rf $(OBJS)
