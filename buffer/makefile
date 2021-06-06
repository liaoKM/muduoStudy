
BASE_OPTS = -pthread

DEBUG_OPTS = -g -O0 -fno-omit-frame-pointer -fsanitize=address  # -DMOODYCAMEL_QUEUE_INTERNAL_DEBUG
RELEASE_OPTS = -O2 -g -DNDEBUG 

ifeq ($(ver), debug)
	CXXFLAGS=$(DEBUG_OPTS) $(BASE_OPTS)
else
	CXXFLAGS=$(RELEASE_OPTS) $(BASE_OPTS)
endif

all: buffer.o 

buffer.o: buffer.cpp ../include/buffer.h
	g++ $(CXXFLAGS) -c $< -o $@


test: test.cpp buffer.o
	g++ $(CXXFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm buffer.o test