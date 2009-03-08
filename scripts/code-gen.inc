#!/bin/bash

# Output file comments.
function file-comments() {
    echo "/// \\file $1"
    echo "/// \\brief $2"
    echo "/// \\author $AUTHOR"
    DAY=`date +%-d`
    DAYMOD=$(($DAY % 10))
    if [ DAYMOD == 1 ]; then
        DAYORD="${DAY}st"
    elif [ DAYMOD == 2 ]; then
        DAYORD="${DAY}nd"
    elif [ DAYMOD == 3 ]; then
        DAYORD="${DAY}rd"
    else
        DAYORD="${DAY}th"
    fi
    echo "/// \\date $DAYORD `date +"%B %Y"`"
    echo "///"
    echo "/// Copyright (c) `date +%Y` $AUTHOR. All rights reserved."
    echo "///"
    echo
    echo
}

# Add opening header guard.
function open-header-guard() {
    GUARDNAME=`echo $1 | tr [a-z] [A-Z] | sed 's/\./_/'`
    echo "#ifndef $GUARDNAME"
    echo "#define $GUARDNAME"
    echo
    echo
}

# Add closing header guard.
function close-header-guard() {
    GUARDNAME=`echo $1 | tr [a-z] [A-Z] | sed 's/\./_/'`
    echo "#endif  // $GUARDNAME"
    echo
}


