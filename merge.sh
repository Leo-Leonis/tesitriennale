#!/bin/bash

Tag=${1}

Files=$(find output_${Tag} | grep run | grep ${Tag} | grep root)

hadd output_${Tag}/merged_mode${Tag}.root $Files

