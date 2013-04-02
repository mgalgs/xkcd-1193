#!/bin/bash

# Your attempt should be the first (and only) argument:
#
# ./sendit.sh myattempt

tmpfile=$(mktemp)
echo "hashable=$1" > $tmpfile
echo "using $tmpfile"
cat $tmpfile
cmd="curl -s -X POST --data \"@${tmpfile}\" http://almamater.xkcd.com/?edu=usu.edu"
echo "running: $cmd"
eval $cmd
rm $tmpfile
