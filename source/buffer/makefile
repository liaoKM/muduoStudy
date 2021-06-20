
all: $(OBJDIR)/buffer.o 

$(OBJDIR)/buffer.o : buffer.cpp ../include/buffer.h
#	@printf "[print test]:$(OBJDIR)\n"
	g++ $(CXXFLAGS) -c $< -o $@


test: test.cpp $(OBJDIR)/buffer.o
	g++ $(CXXFLAGS) $^ -o $@

.PHONY: clean

clean:
	-rm $(OBJDIR)/buffer.o test