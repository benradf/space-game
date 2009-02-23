# MMOEngine Global Makefile
#
# make PLATFORM="mingw" PACKAGES="lua"

PLATFORMS=linux mingw
PACKAGES=enet argtable lua

PATH+=:$(shell pwd)/mingw/usr/bin

all: build install

include scripts/generic.mk

define RULE_DEPS
$(eval $(1): $(foreach PKG,$(PACKAGES),$(foreach PLT,$(PLATFORMS),$(PKG)-$(PLT)-$(1))))
.PHONY: $(1)
endef

$(call RULE_DEPS,extract)
$(call RULE_DEPS,build)
$(call RULE_DEPS,install)
$(call RULE_DEPS,distrib)
$(call RULE_DEPS,clean)

$(foreach PKG,$(PACKAGES), \
	$(if $(shell ls scripts/$(PKG).mk 2>/dev/null), \
		$(eval include scripts/$(PKG).mk) \
	, \
		$(foreach PLT,$(PLATFORMS), \
			$(eval $(call RULE_SET,$(PKG),$(PLT))) \
		) \
	) \
)

.PHONY: all

