CC:= g++ -fopenmp
OBJS:= bubble_sort merge_sort selection_sort insertion_sort
CXXFLAGS:= -Wall -g
LDLIBS:= -ltsal

all: $(OBJS)

.PHONY: clean
clean:
	rm -rf $(OBJS)
