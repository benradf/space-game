#!/bin/bash

function cp-dlls() {
    pe_file=$1
    target_dir=$2
    search_dirs=$3

    strings $pe_file | grep "\.dll" | while read dll; do
        if [ ! -e $target_dir/$dll ]; then
            for search_dir in $search_dirs; do
                if [ -f $search_dir/$dll ]; then
                    cp -v $search_dir/$dll $target_dir
                    cp-dlls "$search_dir/$dll" "$target_dir" "$search_dirs"
                fi
            done
        fi
    done
}

rm -f $2/*.dll
cp-dlls $@

