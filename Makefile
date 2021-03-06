#
# MMOEngine Global Makefile
#
# Usage: make PLATFORM="mingw" PACKAGES="lua" all
#

# Default platforms.
PLATFORMS=linux mingw

# Default packages.
ifneq "$(origin PACKAGES)" "undefined"
else 
ifneq "$(origin COMPONENTS)" "undefined"
else
ifneq "$(origin DATA)" "undefined"
else
include scripts/packages.mk
endif
endif
endif

# Do not build these combinations.
include scripts/blacklist.mk

# Packages, components and in one list.
COMBINED=$(PACKAGES) $(COMPONENTS) $(DATA)

# GET_HOST(platform)
GET_HOST=$(subst mingw,i386-mingw32msvc,$(subst linux,$(shell uname -m)-linux,$(1)))

# UNPACK(package,path)
define UNPACK
	$(if $(filter %.tar.gz,$(1)), tar -xzvf $(1) -C $(2), \
	$(if $(filter %.tar.bz2,$(1)), tar -xjvf $(1) -C $(2), \
	$(if $(filter %.zip,$(1)), unzip $(1) -d $(2), \
	$(error Unknown package format: $(1)))))
endef

# APPLY_PATCHES(package,platform)
define APPLY_PATCHES
	$(foreach PATCH,$(wildcard common/patch/$(1)*.patch.gz), \
		&& zcat $(PATCH) | patch -p1 -d $(2)/src/$(1)*/)
endef

# GET_BUILD_TOOLS(platform)
GET_BUILD_TOOLS=$(call BUILD_TOOLS,$(subst mingw,i386-mingw32msvc-,$(subst linux,,$(1))))
BUILD_TOOLS=CC="$(1)gcc" CXX="$(1)g++" AR="$(1)ar" RANLIB="$(1)ranlib" STRIP="$(1)strip" AS="$(1)as" DLLTOOL="$(1)dlltool" WINDRES="$(1)windres"

# CHECK_CONFIG_EXEC(package,platform)
CHECK_CONFIG_EXEC=$(call FIX_CONFIG_EXEC,$(2)/src/$(1)*/configure)
FIX_CONFIG_EXEC=if [ -f $(1) -a ! -x $(1) ]; then chmod +x $(1); fi

# PACKAGE_RULES(package,platform)
define PACKAGE_RULES
$(1)-$(2)-extract: 
	@echo -e "\033[01;32m$$@\033[00m"; \
	rm -rvf $(2)/src/$(1)*; mkdir -p $(2)/src && \
	$(call UNPACK,$(wildcard common/pkg/$(1)*),$(2)/src) \
	$(call APPLY_PATCHES,$(1),$(2)) && \
	$(call CHECK_CONFIG_EXEC,$(1),$(2))
$(1)-$(2)-build:
	@echo -e "\033[01;31m$$@\033[00m"; \
	cd $(2)/src/$(1)* && export $(call GET_BUILD_TOOLS,$(2)) && \
	$(if $(shell ls $(2)/src/$(1)*/Makefile 2>/dev/null),, \
		./configure --prefix=$(PWD)/$(2) \
		--host=$(call GET_HOST,$(2)) &&) \
	$(MAKE) $(call GET_BUILD_TOOLS,$(2))
$(1)-$(2)-install:
	@echo -e "\033[01;36m$$@\033[00m"; cd $(2)/src/$(1)* && \
	$(MAKE) $(call GET_BUILD_TOOLS,$(2)) install
$(1)-$(2)-distrib:
$(1)-$(2)-clean:
	@echo -e "\033[01;34m$$@\033[00m"; \
	cd $(2)/src/$(1)* && $(MAKE) clean
$(1)-$(2)-all:
	$(MAKE) $(1)-$(2)-build && \
	$(MAKE) $(1)-$(2)-install
.PHONY: \
	$(1)-$(2)-extract \
	$(1)-$(2)-build \
	$(1)-$(2)-install \
	$(1)-$(2)-distrib \
	$(1)-$(2)-clean \
	$(1)-$(2)-all
endef

# COMPONENT_RULES(component,platform)
define COMPONENT_RULES
$(1)-$(2)-extract: 
$(1)-$(2)-build:
	@echo -e "\033[01;31m$$@\033[00m"; mkdir -p $(2)/tmp/$(1) && \
	cd common/src/$(1) && ../../../scripts/gen-make.sh \
		-i "$(PWD)/$(2)/include ../" -j "$(PWD)/$(2)/lib" \
		-l "`sed -n s/$2\://p .linklibs 2>/dev/null`" \
		-n "`sed -n s/$2\://p .linkname 2>/dev/null`" \
		-f "`sed -n s/$2\://p .cxxflags 2>/dev/null`" \
		-d "`sed -n s/$2\://p .ldflags 2>/dev/null`" \
		-h "`sed -n s/$2\://p .install 2>/dev/null`" \
		-r $(PWD) >Makefile.$(2) && \
	rm -f Makefile && ln -s Makefile.$(2) Makefile && \
	$(MAKE) -f Makefile.$(2) PLATFORM="$(2)" \
	$(call GET_BUILD_TOOLS,$(2))
$(1)-$(2)-install:
	@echo -e "\033[01;36m$$@\033[00m"; cd common/src/$(1) && \
	$(MAKE) -f Makefile.$(2) PLATFORM="$(2)" install
$(1)-$(2)-distrib:
	@echo -e "\033[01;35m$$@\033[00m";
$(1)-$(2)-clean:
	@echo -e "\033[01;34m$$@\033[00m"; cd common/src/$(1) && \
	$(MAKE) -f Makefile.$(2) PLATFORM="$(2)" clean
$(1)-$(2)-all:
	$(MAKE) $(1)-$(2)-build && \
	$(MAKE) $(1)-$(2)-install
.PHONY: \
	$(1)-$(2)-extract \
	$(1)-$(2)-build \
	$(1)-$(2)-install \
	$(1)-$(2)-distrib \
	$(1)-$(2)-clean \
	$(1)-$(2)-all
endef

# DATA_RULES(data,platform)
define DATA_RULES
$(1)-$(2)-extract:
$(1)-$(2)-build:
	@echo -e "\033[01;31m$$@\033[00m"; \
	cd common/data/$(1) && $(MAKE) PLATFORM="$(2)"
$(1)-$(2)-install:
	@echo -e "\033[01;36m$$@\033[00m"; \
	cd common/data/$(1) && $(MAKE) PLATFORM="$(2)" install && \
	cd - && scripts/enum-resources.sh $(2)/share/client/ \
	>$(2)/share/client/resources.cfg
$(1)-$(2)-distrib:
$(1)-$(2)-clean:
	@echo -e "\033[01;34m$$@\033[00m"; \
	cd common/data/$(1) && $(MAKE) PLATFORM="$(2)" clean
$(1)-$(2)-all:
	$(MAKE) $(1)-$(2)-build && \
	$(MAKE) $(1)-$(2)-install
.PHONY: \
	$(1)-$(2)-extract \
	$(1)-$(2)-build \
	$(1)-$(2)-install \
	$(1)-$(2)-distrib \
	$(1)-$(2)-clean \
	$(1)-$(2)-all
endef


# Update environment path variable.
PATH+=:$(PWD)/mingw/usr/bin

# Function for top level rules.
DEF_RULE=$(eval .PHONY: $(1)) $(eval $(1): \
	$(foreach PKG,$(COMBINED),$(foreach PLT,$(PLATFORMS), \
	$(if $(filter-out $(BLACKLIST),$(PKG)-$(PLT)),$(PKG)-$(PLT)-$(1)))))

# Define top level rules.
$(call DEF_RULE,all)
$(call DEF_RULE,extract)
$(call DEF_RULE,build)
$(call DEF_RULE,install)
$(call DEF_RULE,distrib)
$(call DEF_RULE,clean)

# Function for package dependencies.
PKG_DEP=$(foreach PLT,$(PLATFORMS),$(eval \
	$(if $(filter-out $(BLACKLIST),$(1)-$(PLT)), \
	$(if $(filter-out $(BLACKLIST),$(2)-$(PLT)), \
	$(if $(filter $(addsuffix -$(PLT),$(COMBINED)),$(1)-$(PLT)), \
	$(if $(filter $(addsuffix -$(PLT),$(COMBINED)),$(2)-$(PLT)), \
	$(1)-$(PLT)-all: $(2)-$(PLT)-all))))))

# Include dependency rules.
include scripts/deprules.mk

# Specific package rules.
$(foreach PKG,$(PACKAGES), \
	$(if $(shell ls scripts/$(PKG).mk 2>/dev/null), \
		$(eval include scripts/$(PKG).mk) \
	, \
		$(foreach PLT,$(PLATFORMS), \
			$(eval $(call PACKAGE_RULES,$(PKG),$(PLT))) \
		) \
	) \
)

# Specific component rules.
$(foreach CPT,$(COMPONENTS), \
	$(if $(shell ls scripts/$(CPT).mk 2>/dev/null), \
		$(eval include scripts/$(CPT).mk) \
	, \
		$(foreach PLT,$(PLATFORMS), \
			$(eval $(call COMPONENT_RULES,$(CPT),$(PLT))) \
		) \
	) \
)

# Specific data rules.
$(foreach DAT,$(DATA), \
	$(if $(shell ls scripts/$(DAT).mk 2>/dev/null), \
		$(eval include scripts/$(DAT).mk) \
	, \
		$(foreach PLT,$(PLATFORMS), \
			$(eval $(call DATA_RULES,$(DAT),$(PLT))) \
		) \
	) \
)
total:
	$(MAKE) clean
	$(MAKE) extract
	$(MAKE) all

.PHONY: total
