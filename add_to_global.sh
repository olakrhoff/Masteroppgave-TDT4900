#!/bin/zsh

directory="data/results"

output_file="data/global_results.txt"


for file in "$directory"/data*; do
    cat "$file" >> "$output_file"
    rm "$file"
done

echo "All data files have been merged into $output_file"

