# Lua specific makefile

lua-linux-extract:
	rm -rvf linux/src/lua*; tar -xzvf common/pkg/lua*.tar.gz -C linux/src/ $(call APPLY_PATCHES,lua,linux)

lua-mingw-extract:
	rm -rvf mingw/src/lua*; tar -xzvf common/pkg/lua*.tar.gz -C mingw/src/ $(call APPLY_PATCHES,lua,mingw)

lua-linux-build:
	cd linux/src/lua* && make $(call GET_BUILD_TOOLS,linux) linux

lua-mingw-build:
	cd mingw/src/lua* && make $(call GET_BUILD_TOOLS,mingw) mingw

lua-linux-install:
	cd linux/src/lua* && make INSTALL_TOP="`pwd`/../.." install

lua-mingw-install:
	cd mingw/src/lua* && make INSTALL_TOP="`pwd`/../.." install

lua-linux-distrib:
	@echo "lua-linux-distrib: nothing to do"

lua-mingw-distrib:
	@echo "lua-mingw-distrib: nothing to do"

lua-linux-clean:
	cd linux/src/lua* && make clean

lua-mingw-clean:
	cd mingw/src/lua* && make clean

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
	lua-mingw-clean

