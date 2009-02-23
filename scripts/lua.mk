# Lua specific makefile

lua-linux-extract:
	rm -rvf linux/src/lua*; tar -xzvf common/pkg/lua*.tar.gz -C linux/src/

lua-mingw-extract:
	rm -rvf mingw/src/lua*; tar -xzvf common/pkg/lua*.tar.gz -C mingw/src/

lua-linux-build:
	pushd linux/src/lua* && (make linux; popd)

lua-mingw-build:
	pushd mingw/src/lua* && (make mingw; popd)

lua-linux-install:
	pushd linux/src/lua* && (make INSTALL_TOP="`pwd`/../.." install; popd)

lua-mingw-install:
	pushd mingw/src/lua* && (make INSTALL_TOP="`pwd`/../.." install; popd)

lua-linux-distrib:
	@echo "lua-linux-distrib: nothing to do"

lua-mingw-distrib:
	@echo "lua-mingw-distrib: nothing to do"

lua-linux-clean:
	pushd linux/src/lua* && (make clean; popd)

lua-mingw-clean:
	pushd mingw/src/lua* && (make clean; popd)

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

