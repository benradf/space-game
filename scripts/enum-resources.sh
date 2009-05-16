#!/bin/bash

printf "[Bootstrap]\n"
printf "Zip=ogrecore.zip\n"
printf "\n"

printf "[General]\n"
find $1 -name "*.zip" -exec printf "Zip=%s\n" "{}" \; | sed "s;$1;;"

printf "\n[CEGUI]\n"
cd common/data/cegui
for f in `ls -d datafiles/*`; do
    printf "FileSystem=$f\n"
done
