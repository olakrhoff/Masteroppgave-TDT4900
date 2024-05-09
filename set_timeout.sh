#!/bin/bash

# Description: This script will take a path to a timed out single run. It will
# create the temp data for it in 'data/results'.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path/to/file>"
    exit 1
fi

file_path="$1"

if [ ! -f "$file_path" ]; then
    echo "Error: Path '$file_path' is not a file."
    exit 1
fi

file_name=$(basename "$file_path")
temp_result_path="data/results/${file_name}_temp.txt"

options="$(cat $file_path | head -n 2 | tail -n 1 | sed -n 's/ : / /g;p')"
echo "./src/bin/BBCMMS $options -s" > /dev/null
./src/bin/BBCMMS $options "-s" > /dev/null

export_path="$(echo "$options" | sed -n 's/.* -e \(.*.txt\) .*/\1/g;p')"

file_content=$(cat "$export_path" | tail -n 1)
corrected_content=$(echo "$file_content" | sed -n 's/, [0-9]*$/, 4000000000/g;p')
echo "$corrected_content" > "$export_path"

rm "$file_path"
