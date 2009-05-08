#
# MMOEngine Particle Makefile
#
# Usage: make PLATFORM="mingw" all
#

ifeq "$(shell uname -m)" "x86_64"
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib64"
else
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib"
endif

BINPREFIX=$(SETLIBPATH) ../../../linux/bin
BUILDPATH=../../../$(PLATFORM)/tmp/particles
INSTALLPATH=../../../$(PLATFORM)/share

all: $(BUILDPATH)/particles.zip

$(BUILDPATH)/particles.zip: *.particle
	mkdir -p $(BUILDPATH)
	zip -ju $@ $?

install:
	mkdir -p $(INSTALLPATH)/client/particles
	cp -uv $(BUILDPATH)/particles.zip $(INSTALLPATH)/client/particles/

clean:
	rm -rvf $(BUILDPATH)

.PHONY: all install

