# --- Makefile for libcore.a ---
#   Tue Feb 24 03:33:28 GMT 2009

# Set variables.
LIB=libcore.a
NAME=core
ROOT?=/media/raid/projects/programming/mmoengine
BUILDROOT?=$(ROOT)
PLATFORM?=linux
PREFIX=$(ROOT)/$(PLATFORM)
BUILDPATH=$(BUILDROOT)/$(PLATFORM)/tmp/$(NAME)
OBJS= \
	$(BUILDPATH)/log.o
CFLAGS:=$(CFLAGS) \
	-I/media/raid/projects/programming/mmoengine/mingw/include 
CXXFLAGS:=$(CXXFLAGS) \
	-I/media/raid/projects/programming/mmoengine/mingw/include 
LDFLAGS:=$(LDFLAGS) \
	-L/media/raid/projects/programming/mmoengine/mingw/lib 
RANLIB?=ranlib

all: $(BUILDPATH)/$(LIB)

clean:
	rm $(BUILDPATH)/*

# Install link target and headers.
install: $(BUILDPATH)/libcore.a core.hpp exception.hpp foreach.hpp log.hpp
	cp -v $(BUILDPATH)/libcore.a $(PREFIX)/lib
	cp -v core.hpp exception.hpp foreach.hpp log.hpp $(PREFIX)/include

# Link object files.
$(BUILDPATH)/$(LIB): $(OBJS)
	$(AR) cru $(BUILDPATH)/$(LIB) $(OBJS)
	$(RANLIB) $(BUILDPATH)/$(LIB)

# Compile source files.
$(BUILDPATH)/log.o: log.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/log.o log.cpp

# Header dependencies.
core.hpp: log.hpp exception.hpp
	@touch core.hpp
exception.hpp:
	@touch exception.hpp
foreach.hpp:
	@touch foreach.hpp
log.cpp: log.hpp
	@touch log.cpp
log.hpp:
	@touch log.hpp

.PHONY: all clean dist
