BASE_OPTS = -pthread

DEBUG_OPTS = -g -O0 -fno-omit-frame-pointer -fsanitize=address  # -DMOODYCAMEL_QUEUE_INTERNAL_DEBUG
RELEASE_OPTS = -O2 -g -DNDEBUG 

ifeq ($(ver), debug)
	CXXFLAGS=$(DEBUG_OPTS) $(BASE_OPTS)
else
	CXXFLAGS=$(RELEASE_OPTS) $(BASE_OPTS)
endif

utilTest:threadPool.o utilTest.cpp
	g++ $(CXXFLAGS) -o $@ $^

threadPool.o:threadPool.cpp threadPool.h
	g++ $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm utilTest threadPool.o