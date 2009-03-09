#!/bin/bash

PACKAGE="$1"
PLATFORM="$2"
NAME=`echo "$3" | sed 's/[ \t]\+/-/g'`
DIFFOPTS="$4"

FULLSRC=`echo $PLATFORM/src/$PACKAGE*/ | awk '{print $1}'`
SRC=`basename $FULLSRC`

echo -n "creating patch '$NAME' based on '$FULLSRC' ."

mv $PLATFORM/src/$SRC $PLATFORM/src/.$SRC
make PACKAGES="$PACKAGE" PLATFORMS="$PLATFORM" extract 1>/dev/null 2>&1

cd $PLATFORM/src
mv .$SRC $SRC-$NAME
diff $DIFFOPTS -cr $SRC $SRC-$NAME 2>&1 | sed "/Only in $SRC/d" | gzip -c > ../../common/patch/$SRC-$NAME.patch.gz
echo -n "."
rm -rf $SRC-$NAME

echo ". done"

