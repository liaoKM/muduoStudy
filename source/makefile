
OBJS := $(shell find $(OBJDIR) -name *.o)

all: $(BINDIR)/echoClient $(BINDIR)/echoServer

$(BINDIR)/echoClient: echoClient.cpp $(OBJS)
	g++ $(CXXFLAGS) $^ -o $@

$(BINDIR)/echoServer: echoServer.cpp $(OBJS)
	g++ $(CXXFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm $(BINDIR)/echoClient $(BINDIR)/echoServer