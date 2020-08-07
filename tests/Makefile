CXXSTD ?= c++17
CXXFLAGS= -std=$(CXXSTD) -Wall -O
CPPFLAGS=-I..
CXXFLAGS += -MMD

check: test_rectangular test_nocompile
	@./test_rectangular

TEST_OBJS=test_main.o test_rectangular.o test_checked_rectangular.o \
	test_rectangular_iterator.o test_copy_move.o

test_rectangular: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $(TEST_OBJS)

clean:
	rm -f *.o *.d errs test_rectangular

test_nocompile:
	@echo "Compile command: $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c"
	@echo "Compiler version:" `$(CXX) --version`
	@ret=0; for i in test_nc_*.cpp; do \
		echo "Testing $$i for not compiling ...\c" ; \
		if $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $$i >/dev/null 2>&1 ; then \
			echo "Oops! it compiled"; ret=1; else echo "OK" ; \
		fi \
	done; exit $$ret

list_compilers:
	@echo Available Compilers:
	@for p in /usr/bin/*++* /usr/local/bin/*++*; do \
		case "$$p" in *\* | *filt | *lex++*) continue;; esac ;\
		ls -l $$p ; \
		$$p --version 2>&1 | sed "s/^/     /"; \
		echo ; \
	done


# For dependency tracking
# gnu make version
include $(wildcard *.d)
