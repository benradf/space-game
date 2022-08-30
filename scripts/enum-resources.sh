#!/bin/bash
set -eu

printf "[Bootstrap]\n"
printf "Zip=common/data/config/ogrecore.zip\n"
printf "\n"

printf "[General]\n"
find $1 -name "*.zip" -exec printf "Zip=%s\n" "{}" \; | grep -vF ogrecore.zip | sed "s;$1;;"

printf "\n[CEGUI]\n"
for f in `ls -d common/data/cegui/datafiles/*`; do
    printf "FileSystem=$f\n"
done
