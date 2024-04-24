#!/bin/bash

# Description: This script will take a path to a timed out single run. It will
# create the temp data for it in 'data/results'.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path/to/file>"
    exit 1
fi

file_path="$1"
echo "FILE PATH: $file_path"

if [ ! -f "$file_path" ]; then
    echo "Error: Path '$file_path' is not a file."
    exit 1
fi

file_name=$(basename "$file_path")
echo "FILE NAME: $file_name"

temp_result_path="data/results/${file_name}_temp.txt"

options="$(cat $file_path | head -n 2 | tail -n 1 | sed -n 's/ : / /g;p')"
echo "./src/bin/BBCMMS $options -s"
./src/bin/BBCMMS $options "-s"

export_path="$(echo "$options" | sed -n 's/.* -e \(.*.txt\) .*/\1/g;p')"

file_content=$(cat "$export_path" | tail -n 1)
corrected_content=$(echo "$file_content" | sed -n 's/, [0-9]*$/, 1800000000/g;p')
echo "$corrected_content" > "$export_path"

rm "$file"
