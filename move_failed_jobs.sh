#!/bin/bash

#Description: This script will move all logs of jobs
#that did not finish in their allocated time.
#The files are moved from <source> to <destination>.
#If finds these files based on the error message
#the server leaves at the end of the file if there
#was a timeout.

if [ $# -ne 2 ]; then
    echo "Usage: $0 <source_directory> <destination_directory>"
    exit 1
fi

source_directory="$1"
destination_directory="$2"
error_message="slurmstepd: error:"

# Check if the source directory exists
if [ ! -d "$source_directory" ]; then
    echo "Source directory not found: $source_directory"
    exit 1
fi

# Check if the destination directory exists
if [ ! -d "$destination_directory" ]; then
    echo "Destination directory not found: $destination_directory"
    exit 1
fi

files_to_add=()
for file in "$source_directory"/*; do
    files_to_add+=("$file")
done

total_files=${#files_to_add[@]}
counter=0
moved=0

# Loop through all .txt files in the source directory
for file in "${files_to_add[@]}"; do
    # Check if the file exists and is readable
    if [ -f "$file" ] && [ -r "$file" ]; then
        # Check if the last line of the file contains the error message
        if tail -n 1 "$file" | grep -q "$error_message"; then
            mv "$file" "$destination_directory"
            ((moved++))
        fi
    fi
    ((counter++))
    percent=$((100 * counter / total_files))

    echo -ne "\rProgress: $percent% ($counter of $total_files files processed ($moved moved))"
done
echo
