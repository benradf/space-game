#
# MMOEngine Ship Makefile
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

# BUILDPATH(ship, platform)
BUILDPATH=../../../$(2)/tmp/ships/$(1)

# INSTALLPATH(platform)
INSTALLPATH=../../../$(1)/share

all:

# SHIP_RULES(ship, platform)
define SHIP_RULES
$(call BUILDPATH,$(1),$(2))/$(1).zip: $(call BUILDPATH,$(1),$(2))/$(1).mesh $(1)/*.material $(1)/*.tga
	zip -ju $$@ $$?
$(call BUILDPATH,$(1),$(2))/$(1).mesh: $(1)/$(1).mesh.xml
	mkdir -p $(call BUILDPATH,$(1),$(2))
	$(OGREXMLCONV) $$< $$@
	rm -f OgreXMLConverter.log
$(1)/$(1).mesh.xml: $(1)/$(1).blend
$(1)-$(2)-all: \
	$(call BUILDPATH,$(1),$(2))/$(1).zip
$(1)-$(2)-install:
	mkdir -p $(call INSTALLPATH,$(2))/client/ships
	cp -uv $(call BUILDPATH,$(1),$(2))/$(1).zip $(call INSTALLPATH,$(2))/client/ships/
$(1)-$(2)-clean:
	rm -rvf $(call BUILDPATH,$(1),$(2))
all: $(1)-$(2)-all
install: $(1)-$(2)-install
clean: $(1)-$(2)-clean
.PHONY: $(1)-$(2)-all $(1)-$(2)-install
endef

$(foreach ship,$(wildcard */),$(eval $(call SHIP_RULES,$(subst /,,$(ship)),$(PLATFORM))))

.PHONY: all

