#!/bin/bash

seq 1 10 >runs.txt
seq 1 1000 >runs.txt
seq 1 100 >runs.txt

rm -r output
root -l -b -q 'main142.cpp+(0)'
sleep 1
date >donelist.txt

# A
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 0' -- {}
# B
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 1' -- {}
# C
# cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 2' -- {}
# D
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 3' -- {}
# E
# cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 4' -- {}
# F
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 5' -- {}
