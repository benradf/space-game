#!/bin/bash

PLATFORM="$1"
MAKETARGET="$2"

function make_with_vars() {
    make CXX="$CXX" CC="$CC" AR="$AR" RANLIB="$RANLIB" $@
}

function alert() {
    echo -e "\033[01;31m$1\033[00m"
}

function config_make_install() {
    pushd $PLATFORM/src/$1*
    case $PLATFORM in
        linux) 
            export \
                CXX="g++" \
                CC="gcc" \
                AR="ar" \
                RANLIB="ranlib"
            ;;
        mingw) 
            export \
                CXX="i386-mingw32msvc-g++" \
                CC="i386-mingw32msvc-gcc" \
                AR="i386-mingw32msvc-ar" \
                RANLIB="i386-mingw32msvc-strip" 
            ;;
    esac
    ./configure \
        --prefix=$PWD/../../ \
        --host=$HOST && \
    make_with_vars $MAKETARGET && \
    make install
    if [ $? != 0 ]; then
        alert "config_make_install: failed to build $1"
        exit
    fi
    popd
}

function make_install_lua() {
    pushd $PLATFORM/src/lua*
    case $PLATFORM in
        linux) 
            export \
                CXX="g++" \
                CC="gcc" \
                AR="ar rcu" \
                RANLIB="ranlib" 
            ;;
        mingw) 
            export \
                CXX="i386-mingw32msvc-g++" \
                CC="i386-mingw32msvc-gcc" \
                AR="i386-mingw32msvc-ar rcu" \
                RANLIB="i386-mingw32msvc-ranlib" 
            ;;
    esac
    make_with_vars $PLATFORM $MAKETARGET
    if [ $? != 0 ]; then
        alert "make_install_lua: failed to build lua"
        exit
    fi
    if [ $PLATFORM == "mingw" ]; then
        cp -v src/*.exe ../../bin && \
        cp -v src/*.dll ../../lib && \
        cp -v src/*.a ../../lib && \
        cp -v src/lua.h src/lualib.h src/lauxlib.h ../../include
        if [ $? != 0 ]; then
            alert "make_install_lua: failed to install lua"
            exit
        fi
    else
        make INSTALL_TOP="$PWD/../../" install
    fi
    popd
}

if [ -z $PLATFORM ]; then
    echo "Usage: build-deps PLATFORM [TARGET]"
    echo "Build dependency libraries for PLATFORM."
    exit
fi

case $PLATFORM in
    linux) HOST="`uname -m`-linux" ;;
    mingw) HOST="i386-mingw32" ;;
esac

export PATH="$PWD/$PLATFORM/usr/bin:$PATH"

config_make_install argtable
config_make_install enet
make_install_lua


