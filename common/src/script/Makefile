# --- Makefile for libscript.a ---
#   Tue Feb 24 03:33:29 GMT 2009

# Set variables.
LIB=libscript.a
NAME=script
ROOT?=/media/raid/projects/programming/mmoengine
BUILDROOT?=$(ROOT)
PLATFORM?=linux
PREFIX=$(ROOT)/$(PLATFORM)
BUILDPATH=$(BUILDROOT)/$(PLATFORM)/tmp/$(NAME)
OBJS= \
	$(BUILDPATH)/luaextlib.o \
	$(BUILDPATH)/luaobject.o \
	$(BUILDPATH)/luautil.o
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
install: $(BUILDPATH)/libscript.a script.hpp luaextlib.hpp luaobject.hpp luautil.hpp
	cp -v $(BUILDPATH)/libscript.a $(PREFIX)/lib
	cp -v script.hpp luaextlib.hpp luaobject.hpp luautil.hpp $(PREFIX)/include

# Link object files.
$(BUILDPATH)/$(LIB): $(OBJS)
	$(AR) cru $(BUILDPATH)/$(LIB) $(OBJS)
	$(RANLIB) $(BUILDPATH)/$(LIB)

# Compile source files.
$(BUILDPATH)/luaextlib.o: luaextlib.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/luaextlib.o luaextlib.cpp
$(BUILDPATH)/luaobject.o: luaobject.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/luaobject.o luaobject.cpp
$(BUILDPATH)/luautil.o: luautil.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/luautil.o luautil.cpp

# Header dependencies.
luaextlib.cpp: luaextlib.hpp
	@touch luaextlib.cpp
luaextlib.hpp: luaobject.hpp
	@touch luaextlib.hpp
luaobject.cpp: luaobject.hpp
	@touch luaobject.cpp
luaobject.hpp:
	@touch luaobject.hpp
luautil.cpp: luautil.hpp
	@touch luautil.cpp
luautil.hpp: luaobject.hpp
	@touch luautil.hpp
script.hpp: luaobject.hpp luautil.hpp luaextlib.hpp
	@touch script.hpp

.PHONY: all clean dist
