#
# MMOEngine Map Makefile
#
# Usage: make PLATFORM="mingw" all
#

ifeq "$(shell uname -m)" "x86_64"
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib64"
else
SETLIBPATH=LD_LIBRARY_PATH="../../../linux/lib:../../../linux/usr/lib"
endif

BINPREFIX=$(SETLIBPATH) ../../../linux/bin
OGREXMLCONV=$(BINPREFIX)/OgreXMLConverter
ZONEBUILD=$(BINPREFIX)/zonebuild

# BUILDPATH(map, platform)
BUILDPATH=../../../$(2)/tmp/maps/$(1)

# INSTALLPATH(platform)
INSTALLPATH=../../../$(1)/share

all:

# MAP_RULES(map, platform)
define MAP_RULES
$(call BUILDPATH,$(1),$(2))/$(1).zip: $(call BUILDPATH,$(1),$(2))/$(1).mesh $(1)/*.material $(1)/*.tga
	zip -ju $$@ $$?
$(call BUILDPATH,$(1),$(2))/$(1).mesh: $(1)/$(1).mesh.xml
	mkdir -p $(call BUILDPATH,$(1),$(2))
	$(OGREXMLCONV) $$< $$@
	rm -f OgreXMLConverter.log
$(1)/$(1).mesh.xml: $(1)/$(1).blend
$(call BUILDPATH,$(1),$(2))/$(1).dat: $(1)/$(1).collision.xml
	$(ZONEBUILD) $$< $$@
$(1)/$(1).collision.xml: $(1)/$(1).blend
$(1)-$(2)-all: \
	$(call BUILDPATH,$(1),$(2))/$(1).zip \
	$(call BUILDPATH,$(1),$(2))/$(1).dat
$(1)-$(2)-install:
	mkdir -p $(call INSTALLPATH,$(2))/client/maps
	cp -uv $(call BUILDPATH,$(1),$(2))/$(1).zip $(call INSTALLPATH,$(2))/client/maps/
	cp -uv $(call BUILDPATH,$(1),$(2))/$(1).dat $(call INSTALLPATH,$(2))/client/maps/
	mkdir -p $(call INSTALLPATH,$(2))/server/maps
	cp -uv $(call BUILDPATH,$(1),$(2))/$(1).dat $(call INSTALLPATH,$(2))/server/maps/
$(1)-$(2)-clean:
	rm -rvf $(call BUILDPATH,$(1),$(2))
all: $(1)-$(2)-all
install: $(1)-$(2)-install
clean: $(1)-$(2)-clean
.PHONY: $(1)-$(2)-all $(1)-$(2)-install
endef

$(foreach map,$(wildcard */),$(eval $(call MAP_RULES,$(subst /,,$(map)),$(PLATFORM))))

.PHONY: all

