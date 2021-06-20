
all: $(OBJDIR)/channel.o  $(OBJDIR)/tcp_connection.o $(OBJDIR)/connector.o $(OBJDIR)/acceptor.o

$(OBJDIR)/channel.o: channel.cpp ../include/channel.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/tcp_connection.o: tcp_connection.cpp ../include/tcp_connection.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/connector.o: connector.cpp ../include/connector.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/acceptor.o: acceptor.cpp ../include/acceptor.h
	g++ $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm $(OBJDIR)/channel.o $(OBJDIR)/tcp_connection.o $(OBJDIR)/connector.o $(OBJDIR)/acceptor.o