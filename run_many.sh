#!/bin/bash

seq 1 1000 >runs.txt

root -l -b -q 'main142.cpp++(0)'
date >donelist.txt
cat "runs.txt" | xargs -n1 -P 112 -I{} sh -c './run_single.sh "$1"' -- {}
