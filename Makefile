CC		= mpic++
CXXFLAGS= -Wall -Wextra -Werror -std=c++14 -pedantic -fPIC -I$(INCLUDE_FLDR)
LDLIBS 	= -l$(LIB_NAME)
LDFLAGS = -L.

SRC_FLDR=src
SAMPLE_FLDR=sample
INCLUDE_FLDR=include
FOLDERS=$(SAMPLE_FLDR) $(SRC_FLDR)

LIB_NAME=algorep
SUM_BINARY=$(addprefix $(SAMPLE_FLDR)/, map_reduce_sum/sum)

LIB_OBJS=src/data/allocator.o src/algorep.o

#VPATH=$(SRC_FLDR);$(SRC_FLDR)/data

lib$(LIB_NAME).so: $(LIB_OBJS)
	$(CC) $(CXXFLAGS) -shared -o $@ $^

##
# Samples
##

$(SUM_BINARY): lib$(LIB_NAME).so $(SUM_BINARY).o

##
# Misc
##

clean:
	$(RM) lib$(LIB_NAME).so $(LIB_OBJS)
	$(RM) $(SUM_BINARY).o $(SUM_BINARY)

format:
	find $(FOLDERS) -name "*.cpp" -o -name "*.h" | xargs clang-format -i

.PHONY: clean format
