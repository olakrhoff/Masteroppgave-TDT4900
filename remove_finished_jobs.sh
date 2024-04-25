#!/bin/bash

#Description: This script will remove all logs for finished jobs.

confirmation_message="Job finished"

directory="output"

counter=0

# Check if the source directory exists
if [ ! -d "$directory" ]; then
    echo "Directory not found: $directory"
    exit 1
fi

# Loop through all .txt files in the source directory
for file in "$directory"/*.txt; do
    # Check if the file exists and is readable
    if [ -f "$file" ] && [ -r "$file" ]; then
        # Check if the last line of the file contains the confirmation message
        if tail -n 1 "$file" | grep -q "$confirmation_message"; then
            rm "$file"
            ((counter++))
        fi
    fi
done

echo  "$counter"