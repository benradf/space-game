# --- Makefile for server ---
#   Tue Feb 24 03:12:05 GMT 2009

# Set variables.
BIN=server
NAME=server
ROOT?=/media/raid/projects/programming/mmoengine
BUILDROOT?=$(ROOT)
PLATFORM?=linux
PREFIX=$(ROOT)/$(PLATFORM)
BUILDPATH=$(BUILDROOT)/$(PLATFORM)/tmp/$(NAME)
OBJS= \
	$(BUILDPATH)/concurrency.o \
	$(BUILDPATH)/idle.o \
	$(BUILDPATH)/mailbox.o \
	$(BUILDPATH)/network.o \
	$(BUILDPATH)/scriptmodule.o \
	$(BUILDPATH)/server.o \
	$(BUILDPATH)/settings.o \
	$(BUILDPATH)/zone.o
CFLAGS:=$(CFLAGS) \
	-I/media/raid/projects/programming/mmoengine/linux/include 
CXXFLAGS:=$(CXXFLAGS) \
	-I/media/raid/projects/programming/mmoengine/linux/include 
LDFLAGS:=$(LDFLAGS) \
	-L/media/raid/projects/programming/mmoengine/linux/lib 
RANLIB?=ranlib

all: $(BUILDPATH)/$(BIN)

clean:
	rm $(BUILDPATH)/*

# Install link target and headers.
install: $(BUILDPATH)/server 
	cp -v $(BUILDPATH)/server $(PREFIX)/bin

# Link object files.
$(BUILDPATH)/$(BIN): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(BUILDPATH)/$(BIN) $(OBJS) -lcore -lscript -lnet -llua -lenet -largtable2 -lpthread -ldl 

# Compile source files.
$(BUILDPATH)/concurrency.o: concurrency.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/concurrency.o concurrency.cpp
$(BUILDPATH)/idle.o: idle.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/idle.o idle.cpp
$(BUILDPATH)/mailbox.o: mailbox.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/mailbox.o mailbox.cpp
$(BUILDPATH)/network.o: network.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/network.o network.cpp
$(BUILDPATH)/scriptmodule.o: scriptmodule.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/scriptmodule.o scriptmodule.cpp
$(BUILDPATH)/server.o: server.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/server.o server.cpp
$(BUILDPATH)/settings.o: settings.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/settings.o settings.cpp
$(BUILDPATH)/zone.o: zone.cpp
	$(CXX) -c $(CXXFLAGS) -o $(BUILDPATH)/zone.o zone.cpp

# Header dependencies.
autolock.hpp: lock.hpp
	@touch autolock.hpp
concurrency.cpp: concurrency.hpp
	@touch concurrency.cpp
concurrency.hpp: lock.hpp
	@touch concurrency.hpp
idle.cpp: idle.hpp
	@touch idle.cpp
idle.hpp: concurrency.hpp localstore.hpp
	@touch idle.hpp
localstore.hpp: lock.hpp autolock.hpp
	@touch localstore.hpp
lock.hpp:
	@touch lock.hpp
luarpc.hpp:
	@touch luarpc.hpp
mailbox.cpp: mailbox.hpp
	@touch mailbox.cpp
mailbox.hpp: autolock.hpp
	@touch mailbox.hpp
network.cpp: network.hpp
	@touch network.cpp
network.hpp: concurrency.hpp settings.hpp scriptmodule.hpp
	@touch network.hpp
peer.hpp:
	@touch peer.hpp
scriptmodule.cpp: scriptmodule.hpp settings.hpp network.hpp
	@touch scriptmodule.cpp
scriptmodule.hpp: concurrency.hpp autolock.hpp luarpc.hpp
	@touch scriptmodule.hpp
server.cpp: concurrency.hpp settings.hpp scriptmodule.hpp network.hpp idle.hpp
	@touch server.cpp
settings.cpp: settings.hpp
	@touch settings.cpp
settings.hpp:
	@touch settings.hpp
zone.cpp:
	@touch zone.cpp
zone.hpp:
	@touch zone.hpp

.PHONY: all clean dist
