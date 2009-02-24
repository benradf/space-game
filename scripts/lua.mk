# Lua specific makefile.

lua-linux-extract:
	@echo -e "\033[01;32m$@\033[00m"; rm -rvf linux/src/lua*; \
	tar -xzvf common/pkg/lua*.tar.gz -C linux/src/ $(call APPLY_PATCHES,lua,linux)

lua-mingw-extract:
	@echo -e "\033[01;32m$@\033[00m"; rm -rvf mingw/src/lua*; \
	tar -xzvf common/pkg/lua*.tar.gz -C mingw/src/ $(call APPLY_PATCHES,lua,mingw)

lua-linux-build:
	@echo -e "\033[01;31m$@\033[00m"; cd linux/src/lua* && \
	$(MAKE) $(call GET_BUILD_TOOLS,linux) linux

lua-mingw-build:
	@echo -e "\033[01;31m$@\033[00m"; cd mingw/src/lua* && \
	$(MAKE) $(call GET_BUILD_TOOLS,mingw) mingw

lua-linux-install:
	@echo -e "\033[01;36m$@\033[00m"; cd linux/src/lua* && \
	$(MAKE) INSTALL_TOP="`pwd`/../.." install

lua-mingw-install:
	@echo -e "\033[01;36m$@\033[00m"; cd mingw/src/lua* && \
	$(MAKE) INSTALL_TOP="`pwd`/../.." install

lua-linux-distrib:

lua-mingw-distrib:

lua-linux-clean:
	@echo -e "\033[01;34m$@\033[00m"; \
	cd linux/src/lua* && $(MAKE) clean

lua-mingw-clean:
	@echo -e "\033[01;34m$@\033[00m"; \
	cd mingw/src/lua* && $(MAKE) clean

lua-linux-all:
	$(MAKE) lua-linux-build && \
	$(MAKE) lua-linux-install

lua-mingw-all:
	$(MAKE) lua-mingw-build && \
	$(MAKE) lua-mingw-install

.PHONY: \
	lua-linux-extract \
	lua-mingw-extract \
	lua-linux-build \
	lua-mingw-build \
	lua-linux-install \
	lua-mingw-install \
	lua-linux-distrib \
	lua-mingw-distrib \
	lua-linux-clean \
	lua-mingw-clean \
	lua-linux-all \
	lua-mingw-all

