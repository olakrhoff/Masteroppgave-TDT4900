#!/bin/bash

# Description: This script takes a path to a file. It will find the last
# configuration of optimisations that ran on this file and schedule a single job
# for each config that did not finish or start running.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path/to/interval/log/file>"
    exit 1
fi

log_file="$1"

if [ ! -f "$log_file" ]; then
	echo "File given was not a file: $log_file"
	exit 1
fi

filtered_line=$(grep 'Starting branch and bound for batch:' $log_file | tail -n 1)

last_instance=$(echo "$filtered_line" | sed -n 's/.*\(data*\)/\1/p')

last_opt=$(grep './src' $log_file | tail -n 1 | sed -n 's/.* data\/.*txt \(.*\)/\1/p')

counter=$(./create_jobs_single.sh "$last_instance" "$last_opt")

rm $log_file

echo "$counter"
