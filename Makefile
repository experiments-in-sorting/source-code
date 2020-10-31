CXXFLAGS:= -Wall -g
LDFLAGS:= -l tsal
OBJS:= bubble_sort merge_sort selection_sort insertion_sort

all: $(OBJS)

clean:
	rm -rf $(OBJS)
