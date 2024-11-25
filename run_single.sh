#!/bin/bash

RunId=${1-0}
RunningMode=${2-1}


float_result=$(( RunId - 1 ))
float_result=$(( float_result % 100 ))
float_result=$(( float_result * 30))
#float_result=1

echo "Running $RunId: sleeping $float_result"
sleep $float_result


#if [[ $RunId == "1" ]]; then
#    echo "Running $RunId"
#else
#    previousRun=output/run_$((RunId - 1))/started
#    echo "$RunId waiting for $previousRun"
#    while true; do
#        if [[ -f $previousRun ]]; then
#            break
#        fi
#        sleep 5
#    done
#    echo "Running $RunId"
#fi

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

Cmd="root -l -b -q 'main142.cpp+(1000000, 1, ${RunId}, ${RunningMode})'"
echo $Cmd >>log
eval $Cmd >>log

cd -

echo $RunId >>donelist.txt
