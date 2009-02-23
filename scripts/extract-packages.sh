#!/bin/bash

PLATFORM="$1"

if [ -z $PLATFORM ]; then
    echo "Usage: extract-packages PLATFORM"
    echo "Extract packages to PLATFORM directory."
    exit
fi

mkdir -p $PLATFORM/src

for f in `ls --color=none common/pkg/*.tar.gz`; do
    tar -xzvf $f -C $PLATFORM/src/
done

for f in `ls --color=none common/patch/*.patch.gz`; do
    zcat $f | patch -p0 -d $PLATFORM/src/
done

