#!/bin/bash

RunId=${1-0}


float_result=$(( RunId % 100 ))
float_result=$(( float_result * 60))

echo "Running $RunId: sleeping $float_result"
sleep $float_result

outDir="output/run_${RunId}/"
if [ -d $outDir ]; then
    echo $outDir already there
    cd $outDir
else
    mkdir -p $outDir
    cd $outDir
    ln -s ../../main142* .
fi

date > log

Cmd="root -l -b -q 'main142.cpp+(1000000, 1, ${RunId}, 0)'"
echo $Cmd >> log
eval $Cmd >> log

cd -

echo $RunId >>donelist.txt
