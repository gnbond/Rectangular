CXXSTD=c++03
CXXFLAGS=-std=$(CXXSTD) -Wall -O2
CXXFLAGS += -MMD

check: test_cpp03
	@./test_cpp03

OBJS=test_cpp03.o 

test_cpp03: $(OBJS)
	$(LINK.cc) -o $@ $(OBJS)

clean:
	rm -f *.o *.d errs test_cpp03

# For dependency tracking
# gnu make version
include $(wildcard *.d)

