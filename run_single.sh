#!/bin/bash

RunId=${1-0}

if [[ $RunId == "1" ]]; then
    echo "Running $RunId"
else
    previousRun=output/run_$((RunId - 1))/started
    echo "$RunId waiting for $previousRun"
    while true; do
        if [[ -f $previousRun ]]; then
            break
        fi
        sleep 1
    done
    echo "Running $RunId"
fi

outDir="output/run_${RunId}/"
if [ -d $outDir ]; then
    echo $outDir already there
    cd $outDir
else
    mkdir -p $outDir
    cd $outDir
    ln -s ../../main142* .
fi

date >log

Cmd="root -l -b -q 'main142.cpp+(1000000, 1, ${RunId}, 0)'"
Cmd="root -l -b -q 'main142.cpp+(0, 1, ${RunId}, 0)'"
echo $Cmd >>log
eval $Cmd >>log

cd -

echo $RunId >>donelist.txt
