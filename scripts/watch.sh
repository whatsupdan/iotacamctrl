#!/bin/sh

# get the current path
CURPATH=$(pwd)
#--exclude ".*(\.git|\.idea)"
inotifywait -r -m --exludei ".*(\.git|\.idea|build)" "$CURPATH" |
   while read path _ file; do
       echo $path$file modified
       make install
   done














