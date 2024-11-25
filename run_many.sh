#!/bin/bash

seq 1 10 >runs.txt
seq 1 1000 >runs.txt
seq 1 100 >runs.txt

root -l -b -q 'main142.cpp+(0)'
sleep 1
date >donelist.txt

rm -r output
# A
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 0' -- {}
mv output output_A
# B
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 1' -- {}
mv output output_B
# C
# cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 2' -- {}
mv output output_C
# D
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 3' -- {}
mv output output_D
# E
# cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 4' -- {}
mv output output_E
# F
cat "runs.txt" | xargs -n1 -P ${1-100} -I{} sh -c './run_single.sh "$1" 5' -- {}
mv output output_F


