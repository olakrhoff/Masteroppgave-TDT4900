#!/bin/bash

#Description: This script will remove all logs for finished jobs.

confirmation_message="Job finished"

directory="output"

# Check if the source directory exists
if [ ! -d "$directory" ]; then
    echo "Directory not found: $directory"
    exit 1
fi


files_to_add=()
for file in "$directory"/*; do
    files_to_add+=("$file")
done

total_files=${#files_to_add[@]}

counter=0
removed=0



# Loop through all .txt files in the source directory
for file in "${files_to_add[@]}"; do
    # Check if the file exists and is readable
    if [ -f "$file" ] && [ -r "$file" ]; then
        # Check if the last line of the file contains the confirmation message
        if tail -n 1 "$file" | grep -q "$confirmation_message"; then
            rm "$file"
            ((removed++))
        fi
    fi
    ((counter++))
    percent=$((100 * counter / total_files))

    echo -ne "\rProgress: $percent% ($counter of $total_files files processed ($removed removed))"
done
echo
