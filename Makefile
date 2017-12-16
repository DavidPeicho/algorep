CC=g++
CXXFLAGS=-Wall -Wextra -Werror -std=c++14 -pedantic -I$(INCLUDE_FLDR)

SRC_FLDR=src
SAMPLE_FLDR=sample
INCLUDE_FLDR=include
FOLDERS=$(SAMPLE_FLDR) $(SRC_FLDR)

SUM_BINARY=$(addprefix $(SAMPLE_FLDR)/, map_reduce_sum/sum)

VPATH=$(SRC_FLDR)

algorep: $(OBJS)

$(SUM_BINARY): $(SUM_BINARY).o

clean:
	$(RM) $(SRC_OBJS)
	$(RM) $(SUM_BINARY).o $(SUM_BINARY)

format:
	find $(FOLDERS) -name "*.cpp" -o -name "*.h" | xargs clang-format -i

.PHONY: clean format
