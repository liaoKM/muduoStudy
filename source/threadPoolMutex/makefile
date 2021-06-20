

$(OBJDIR)/threadPool.o:threadPool.cpp ../include/threadPool.h
	g++ $(CXXFLAGS) -c $< -o $@

test:threadPool.o utilTest.cpp
	g++ $(CXXFLAGS) -o $@ $^

.PHONY: clean

clean:
	-rm utilTest threadPool.o