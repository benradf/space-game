#
# MMOEngine Material Makefile
#
# Usage: make PLATFORM="mingw" all
#

ifeq "$(shell uname -m)" "x86_64"
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib64"
else
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib"
endif

BINPREFIX=$(SETLIBPATH) ../../../linux/bin
BUILDPATH=../../../$(PLATFORM)/tmp/materials
INSTALLPATH=../../../$(PLATFORM)/share

all: $(BUILDPATH)/materials.zip

$(BUILDPATH)/materials.zip: *.material
	mkdir -p $(BUILDPATH)
	zip -ju $@ $?

install:
	mkdir -p $(INSTALLPATH)/client/materials
	cp -uv $(BUILDPATH)/materials.zip $(INSTALLPATH)/client/materials/

clean:
	rm -rvf $(BUILDPATH)

.PHONY: all install

