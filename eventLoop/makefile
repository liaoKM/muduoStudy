
BASE_OPTS = -pthread

DEBUG_OPTS = -g -O0 -fno-omit-frame-pointer -fsanitize=address  # -DMOODYCAMEL_QUEUE_INTERNAL_DEBUG
RELEASE_OPTS = -O2 -g -DNDEBUG 

ifeq ($(ver), debug)
	CXXFLAGS=$(DEBUG_OPTS) $(BASE_OPTS)
else
	CXXFLAGS=$(RELEASE_OPTS) $(BASE_OPTS)
endif

all: ethread_pool.o ethread.o eventloop.o

ethread_pool.o: ethread_pool.cpp ../include/ethread_pool.h ../include/ethread.h ../include/eventloop.h
	g++ $(CXXFLAGS) -c $< -o $@

ethread.o: ethread.cpp ../include/ethread.h
	g++ $(CXXFLAGS) -c $< -o $@

eventloop.o: eventloop.cpp ../include/eventloop.h
	g++ $(CXXFLAGS) -c $< -o $@

test: looptest.cpp ethread_pool.o ethread.o eventloop.o
	g++ $(CXXFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm ethread_pool.o ethread.o eventloop.o test