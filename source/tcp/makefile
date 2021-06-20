
all: $(OBJDIR)/tcp_client.o $(OBJDIR)/tcp_server.o

$(OBJDIR)/tcp_client.o: tcp_client.cpp ../include/tcp_client.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/tcp_server.o: tcp_server.cpp ../include/tcp_server.h
	g++ $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm $(OBJDIR)/tcp_client.o $(OBJDIR)/tcp_server.o