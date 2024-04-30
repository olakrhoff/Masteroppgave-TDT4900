#!/bin/bash

directory="data/results"

output_file="data/global_results.txt"

files_to_add=()
for file in "$directory"/*; do
    files_to_add+=("$file")
done

total_files=${#files_to_add[@]}
counter=0

for file in "${files_to_add[@]}"; do
    cat "$file" >> "$output_file"
    rm "$file"
    
    ((counter++))
    percent=$((100 * counter / total_files))

    echo -ne "\rProgress: $percent% ($counter of $total_files files processed)"
done
echo
echo "All data files have been merged into $output_file"

