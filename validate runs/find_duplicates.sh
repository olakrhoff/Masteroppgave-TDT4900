#!/bin/bash

# Check if the input file argument is provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <input_file> <output_file>"
    exit 1
fi

# Assign input file to a variable
input_file=$1
output_file=$2

# Check if the input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' does not exist."
    exit 1
fi

cat "$input_file" | tail -n +2 | sed -n "s/\(.*txt, \).*\([0-1],[ ]*[0-1],[ ]*[0-1],[ ]*[0-5],[ ]*[0-5],[ ]*[0-1],[ ]*[0-1],[ ]*[0-1]\),[ ]*[0-9]*$/\1\2/p" | sort | uniq -c | sort | grep -v "1 " > "$output_file"
