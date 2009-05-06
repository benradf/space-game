#!/bin/bash


function distrib-client-mingw() {
    PATH="$PATH:$PWD/mingw/usr/bin"

    rm -rf mingw/distrib/client
    mkdir -p mingw/distrib/client

    # Client data.
    cp -Rv mingw/share/client/* mingw/distrib/client

    # Client binary.
    cp -v mingw/bin/client.exe mingw/distrib/client

    # Libraries.
    scripts/cp-dll-depends.sh mingw/bin/client.exe mingw/distrib/client "mingw/bin mingw/lib"

    # Ogre plugins.
    mkdir -p mingw/distrib/client/plugins
    cp -v mingw/lib/OGRE/RenderSystem_GL.dll mingw/distrib/client/plugins
    cp -v mingw/lib/OGRE/Plugin_OctreeSceneManager.dll mingw/distrib/client/plugins
    mv mingw/distrib/client/plugins.cfg mingw/distrib/client/plugins.cfg.in
    sed 's/^PluginFolder=.*$/PluginFolder=plugins/;s/^Plugin=\(.*\)\.so$/Plugin=\1\.dll/' \
        mingw/distrib/client/plugins.cfg.in >mingw/distrib/client/plugins.cfg
    rm mingw/distrib/client/plugins.cfg.in

    # Pack debug archive.
    cd mingw/distrib; rar -r a client-debug-`date +%d%m%y-%H%M%S`.rar client/; cd -

    # Strip binaries.
    find mingw/distrib/client '(' -name "*.dll" -or -name "*.exe" ')' -exec i386-mingw32msvc-strip {} \;

    # Pack release archive.
    cd mingw/distrib; rar -r a client-release-`date +%d%m%y-%H%M%S`.rar client/; cd -
}


if [ "$PLATFORM" == "mingw" ] && [ "$COMPONENT" == "client" ]; then
    distrib-client-mingw
else
    echo -e "unable to build distrib package for \033[01;31m$COMPONENT-$PLATFORM\033[00m"
fi

