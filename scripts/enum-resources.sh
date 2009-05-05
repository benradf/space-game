#!/bin/bash

printf "[Bootstrap]\n"
printf "Zip=ogrecore.zip\n"
printf "\n"

printf "[General]\n"
find $1 -name "*.zip" -exec printf "Zip=%s\n" "{}" \; | sed "s;$1;;"

