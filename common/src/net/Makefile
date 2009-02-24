# --- Makefile for libnet.a ---
#   Tue Feb 24 03:12:07 GMT 2009

# Set variables.
LIB=libnet.a
NAME=net
ROOT?=/media/raid/projects/programming/mmoengine
BUILDROOT?=$(ROOT)
PLATFORM?=linux
PREFIX=$(ROOT)/$(PLATFORM)
BUILDPATH=$(BUILDROOT)/$(PLATFORM)/tmp/$(NAME)
OBJS= \
	$(BUILDPATH)/net.o
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
install: $(BUILDPATH)/libnet.a net.hpp
	cp -v $(BUILDPATH)/libnet.a $(PREFIX)/lib
	cp -v net.hpp $(PREFIX)/include

# Link object files.
$(BUILDPATH)/$(LIB): $(OBJS)
	$(AR) cru $(BUILDPATH)/$(LIB) $(OBJS)
	$(RANLIB) $(BUILDPATH)/$(LIB)

# Compile source files.
$(BUILDPATH)/net.o: net.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/net.o net.cpp

# Header dependencies.
net.cpp:
	@touch net.cpp
net.hpp:
	@touch net.hpp

.PHONY: all clean dist
