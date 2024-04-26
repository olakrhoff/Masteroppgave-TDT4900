#!/bin/bash

# Find all file paths to inner folders
# filter out the ones having 'small' in them
# create jobs with all the folders

if [ $# -ne 1 ]; then
    echo "Usage: $0 <root_directory>"
    exit 1
fi

root_directory="$1"
filter="data/interval"

while IFS= read -r directory; do
    if echo "$directory" | grep -q "$filter"; then
	./create_jobs.sh $directory
    fi
done < <(find "$root_directory" -type d)
