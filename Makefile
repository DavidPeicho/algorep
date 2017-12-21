CXX = mpic++
CC = $(CXX)
CXXFLAGS = -Wall -Wextra -Werror -std=c++14 -pedantic -g -fPIC -I./include
LDLIBS = -l$(LIB_NAME)
LDFLAGS = -L.

LIB_NAME=algorep
LIB_OBJS=src/data/allocator.o src/algorep.o src/data/memory.o

lib$(LIB_NAME).so: $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

###############################################################################
# 							       TEST SUITE
###############################################################################

# This is gross because we do not use the implicit rules of Make.
# However, this is used to simplify the usage of Make.

check: test/print test/print_random test/map test/reduce
	sh test/check.sh

test/print: lib$(LIB_NAME).so test/print.o
test/print_random: lib$(LIB_NAME).so test/print_random.o
test/map: lib$(LIB_NAME).so test/map.o
test/reduce: lib$(LIB_NAME).so test/reduce.o

###############################################################################
# 								    SAMPLES
###############################################################################

sample/map_reduce: lib$(LIB_NAME).so sample/map_reduce.o

###############################################################################
# 									 MISC
###############################################################################

clean:
	$(RM) lib$(LIB_NAME).so $(LIB_OBJS)
	$(RM) test/print sample/print.o
	$(RM) test/print_random sample/print_random.o
	$(RM) test/map sample/map.o
	$(RM) test/reduce sample/reduce.o

format:
	find test/ include/ src/ -name "*.cpp" -o -name "*.h" -o -name "*.hxx" | xargs clang-format -i

.PHONY: clean format
