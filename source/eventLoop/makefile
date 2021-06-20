all: $(OBJDIR)/ethread_pool.o $(OBJDIR)/ethread.o $(OBJDIR)/eventloop.o

$(OBJDIR)/ethread_pool.o: ethread_pool.cpp ../include/ethread_pool.h ../include/ethread.h ../include/eventloop.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/ethread.o: ethread.cpp ../include/ethread.h
	g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/eventloop.o: eventloop.cpp ../include/eventloop.h
	g++ $(CXXFLAGS) -c $< -o $@

test: looptest.cpp ethread_pool.o ethread.o eventloop.o
	g++ $(CXXFLAGS) $^ -o $@

.PHONY: clean 

clean:
	-rm ethread_pool.o ethread.o eventloop.o test