#!/bin/bash
find . -type f | grep -v '\.sw.$' | nix-shell --run "
  entr bash -c 'clear; date; echo; bazel build //...'
"
