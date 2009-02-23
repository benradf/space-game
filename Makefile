# MMOEngine Global Makefile
#
# Usage: make PLATFORM="mingw" PACKAGES="lua" all
#

include scripts/generic.mk

# Default platforms.
PLATFORMS=linux mingw

# Default packages.
PACKAGES=enet argtable lua

# Update environment path variable.
PATH+=:$(shell pwd)/mingw/usr/bin

# Function for top level rules.
DEF_RULE=$(eval .PHONY: $(1)) \
	$(eval $(1): $(foreach PKG,$(PACKAGES), \
	$(foreach PLT,$(PLATFORMS),$(PKG)-$(PLT)-$(1))))

# Define top level rules.
$(call DEF_RULE,all)
$(call DEF_RULE,extract)
$(call DEF_RULE,build)
$(call DEF_RULE,install)
$(call DEF_RULE,distrib)
$(call DEF_RULE,clean)

# Function for package dependencies.
PKG_DEP=$(foreach PLT,$(PLATFORMS), \
	$(eval $(1)-$(PLT)-all: $(2)-$(PLT)-all))

# Declare package dependencies.
$(call PKG_DEP,lua,enet)
$(call PKG_DEP,enet,argtable)

# Define specific rules.
$(foreach PKG,$(PACKAGES), \
	$(if $(shell ls scripts/$(PKG).mk 2>/dev/null), \
		$(eval include scripts/$(PKG).mk) \
	, \
		$(foreach PLT,$(PLATFORMS), \
			$(eval $(call RULE_SET,$(PKG),$(PLT))) \
		) \
	) \
)

