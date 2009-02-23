# Generic makefile

# GET_HOST(platform)
GET_HOST=$(subst mingw,i386-mingw32,$(subst linux,$(shell uname -m)-linux,$(1)))

# UNPACK(package,path)
define UNPACK
	$(if $(filter %.tar.gz,$(1)), tar -xzvf $(1) -C $(2), \
	$(if $(filter %.tar.bz2,$(1)), tar -xjvf $(1) -C $(2), \
	$(if $(filter %.zip,$(1)), unzip $(1) -d $(2), \
	$(error Unknown package format: $(1)))))
endef

# RULE_SET(package,platform)
define RULE_SET
$(1)-$(2)-extract: 
	rm -rvf $(2)/src/$(1)*; \
	$(call UNPACK,$(wildcard common/pkg/$(1)*),$(2)/src)
$(1)-$(2)-build:
	pushd $(2)/src/$(1)* && \
	($(if $(shell ls $(2)/src/$(1)*/Makefile 2>/dev/null),,./configure \
		--prefix=$(shell pwd)/$(2) \
		--host=$(call GET_HOST,$(2)) &&) \
	make; popd)
$(1)-$(2)-install:
	pushd $(2)/src/$(1)* && \
	(make install; popd)
$(1)-$(2)-distrib:
	@echo "$(1)-$(2)-distrib"
$(1)-$(2)-clean:
	pushd $(2)/src/$(1)* && \
	(make clean; popd)
.PHONY: \
	$(1)-$(2)-extract \
	$(1)-$(2)-build \
	$(1)-$(2)-install \
	$(1)-$(2)-distrib \
	$(1)-$(2)-clean
endef

