CXXFLAGS= -std=c++17 -Wall -O
CXXFLAGS += -MMD

check: test_rectangular test_nocompile
	@./test_rectangular

TEST_OBJS=test_main.o test_rectangular.o test_checked_rectangular.o test_rectangular_iterator.o
test_rectangular: $(TEST_OBJS)
	$(LINK.cc) -o $@ $(TEST_OBJS)

clean:
	rm -f *.o *.d errs test_rectangular

test_nocompile: 
	@echo "Compile command: $(COMPILE.cc)"
	@ret=0; for i in test_nc_*.cpp; do \
		echo "Testing $$i for not compiling ...\c" ; \
		if $(COMPILE.cc) $$i >/dev/null 2>&1 ; then echo "Oops! it compiled"; ret=1; fi ;\
		echo "OK" ; \
	done; exit $$ret

# For dependency tracking
# gnu make version
include $(wildcard *.d)
