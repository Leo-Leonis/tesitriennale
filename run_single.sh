#!/bin/bash

RunId=${1-0}

echo $RunId

float_result=$(echo "scale=5; $RunId / 100" | bc)
float_result=$(echo "scale=5; $RunId / 1" | bc)
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

Cmd="root -l -b -q 'main142.cpp+(1000000, 1, ${RunId}, 0)' > log"
echo $Cmd
eval $Cmd

cd -

echo $RunId >>donelist.txt
