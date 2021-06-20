
$(OBJDIR)/socket.o: socket.cpp ../include/socket.h
	g++ $(CXXFLAGS) -c $< -o $@

test: testClient testServer

testClient: socket.o testClient.cpp
	g++ $(CXXFLAGS) -o $@ $^

testServer: socket.o testServer.cpp
	g++ $(CXXFLAGS) -o $@ $^