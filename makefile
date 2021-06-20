ROOTDIR := $(shell pwd)
OUTDIR := $(ROOTDIR)/build
OBJDIR := $(OUTDIR)/object
BINDIR := $(OUTDIR)/bin
SOURCEDIR := $(ROOTDIR)/source

BASE_OPTS := -pthread
DEBUG_OPTS := -g -O0 -fno-omit-frame-pointer -fsanitize=address  # -DMOODYCAMEL_QUEUE_INTERNAL_DEBUG
RELEASE_OPTS := -O2 -g -DNDEBUG 

SUBDIRS := $(shell ls $(SOURCEDIR))

ifeq ($(ver), debug)
	CXXFLAGS=$(DEBUG_OPTS) $(BASE_OPTS)
else
	CXXFLAGS=$(RELEASE_OPTS) $(BASE_OPTS)
endif

export OUTDIR OBJDIR BINDIR CXXFLAGS

all:
	@printf "$(SUBDIRS)\n"
	@for sub in $(SUBDIRS); do\
		make -C $(SOURCEDIR)/$$sub;\
	done
	make -C $(SOURCEDIR)

app:
	make -C $(SOURCEDIR)

.PONY: clean

clean:
#	rm $(OBJDIR)/* rm / if OBJDIR is None
	rm $(OUTDIR)/object/*
	rm $(OUTDIR)/bin/*

