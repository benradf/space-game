# Defines rules for generic packages.

# GET_HOST(platform)
GET_HOST=$(subst mingw,i386-mingw32,$(subst linux,$(shell uname -m)-linux,$(1)))

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
BUILD_TOOLS=CC="$(1)gcc" CXX="$(1)g++" AR="$(1)ar" RANLIB="$(1)ranlib"

# RULE_SET(package,platform)
define RULE_SET
$(1)-$(2)-extract: 
	rm -rvf $(2)/src/$(1)*; \
	$(call UNPACK,$(wildcard common/pkg/$(1)*),$(2)/src) \
	$(call APPLY_PATCHES,$(1),$(2))
$(1)-$(2)-build:
	cd $(2)/src/$(1)* && \
	$(if $(shell ls $(2)/src/$(1)*/Makefile 2>/dev/null),,./configure \
		--prefix=$(shell pwd)/$(2) \
		--host=$(call GET_HOST,$(2)) &&) \
	make $(call GET_BUILD_TOOLS,$(2))
$(1)-$(2)-install:
	cd $(2)/src/$(1)* && make install
$(1)-$(2)-distrib:
	@echo "$(1)-$(2)-distrib"
$(1)-$(2)-clean:
	cd $(2)/src/$(1)* && make clean
$(1)-$(2)-all:
	make $(1)-$(2)-build && make $(1)-$(2)-install
.PHONY: \
	$(1)-$(2)-extract \
	$(1)-$(2)-build \
	$(1)-$(2)-install \
	$(1)-$(2)-distrib \
	$(1)-$(2)-clean
endef

