#!/bin/sh

# get the current path
CURPATH=$(pwd)
#--exclude ".*(\.git|\.idea)"
inotifywait -r -m --excludei ".*(\.git|\.idea|build)" "$(pwd)" |
   while read path _ file; do
       echo $path$file modified
       pip install -e .
   done














