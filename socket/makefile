
BASE_OPTS = -pthread

DEBUG_OPTS = -g -O0 -fno-omit-frame-pointer -fsanitize=address  # -DMOODYCAMEL_QUEUE_INTERNAL_DEBUG
RELEASE_OPTS = -O2 -g -DNDEBUG 

ifeq ($(ver), debug)
	CXXFLAGS=$(DEBUG_OPTS) $(BASE_OPTS)
else
	CXXFLAGS=$(RELEASE_OPTS) $(BASE_OPTS)
endif

socket.o: socket.cpp ../include/socket.h
	g++ $(CXXFLAGS) -c $< -o $@

test: testClient testServer

testClient: socket.o testClient.cpp
	g++ $(CXXFLAGS) -o $@ $^

testServer: socket.o testServer.cpp
	g++ $(CXXFLAGS) -o $@ $^