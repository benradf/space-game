#
# MMOEngine Config Makefile
#
# Usage: make PLATFORM="mingw" all
#

ifeq "$(shell uname -m)" "x86_64"
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib64"
else
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib"
endif

BINPREFIX=$(SETLIBPATH) ../../../linux/bin
INSTALLPATH=../../../$(PLATFORM)/share

all: 

install:
	mkdir -p $(INSTALLPATH)/client/
	cp -uv *.cfg *.zip $(INSTALLPATH)/client/

clean:

.PHONY: all install

