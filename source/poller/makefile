all: $(OBJDIR)/epoller.o $(OBJDIR)/poller.o

$(OBJDIR)/epoller.o: EPollPoller.cpp ../include/EpollPoller.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/poller.o: poller.cpp ../include/poller.h
	g++ $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm $(OBJDIR)/epoller.o $(OBJDIR)/poller.o