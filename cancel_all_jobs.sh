#!/bin/bash

# Description: This script will cancel all active jobs


username=$(cat username.txt)

lines=($(squeue -u "$username" | grep "$username" | sed -n "s/[ ]*\([0-9]*\) .*/\1/p"))

for line in "${lines[@]}"; do
    scancel $line
done
