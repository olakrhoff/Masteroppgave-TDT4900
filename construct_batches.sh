#!/bin/bash

# Find all file paths to inner folders
# filter out the ones having 'filter' in them
# create jobs with all the folders

if [ $# -ne 1 ]; then
    echo "Usage: $0 <root_directory>"
    exit 1
fi

root_directory="$1"
filter="data/interval"


directories=($(find "$root_directory" -type f | sed -n 's/\(.*\)\/[a-z]*[0-9]*.txt/\1/p' | uniq))

for directory in "${directories[@]}"; do
    ./create_jobs.sh $directory
done
