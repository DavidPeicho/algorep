CXX = mpic++
CC = $(CXX)
CXXFLAGS = -Wall -Wextra -Werror -std=c++14 -pedantic -g -fPIC -I./include
LDLIBS = -l$(LIB_NAME)
LDFLAGS = -L.

LIB_NAME=algorep

LIB_OBJS=src/data/allocator.o src/algorep.o src/data/memory.o

#VPATH=$(SRC_FLDR);$(SRC_FLDR)/data

lib$(LIB_NAME).so: $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^

##
# Samples
##

# This is gross because we do not use the implicit rules of Make.
# However, this is used to simplify the usage of Make.

sample/print: lib$(LIB_NAME).so sample/print.o
sample/print_random: lib$(LIB_NAME).so sample/print_random.o
sample/map: lib$(LIB_NAME).so sample/map.o

##
# Misc
##

clean:
	$(RM) lib$(LIB_NAME).so $(LIB_OBJS)
	$(RM) sample/print sample/print.o
	$(RM) sample/print_random sample/print_random.o
	$(RM) sample/map sample/map.o

format:
	find sample/ include/ src/ -name "*.cpp" -o -name "*.h" -o -name "*.hxx" | xargs clang-format -i

.PHONY: clean format
