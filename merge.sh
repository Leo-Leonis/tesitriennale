#!/bin/bash

Tag=${1}

Files=$(find output | grep run | grep ${Tag} | grep root)

hadd output/merged_mode${Tag}.root $Files

