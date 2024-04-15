#!/bin/zsh

# Define the directory where the data files are located
directory="data/results"

# Define the output file
output_file="data/global_results.txt"

# Initialize the output file
#> "$output_file"

# Iterate over the data files and append their contents to the output file
for file in "$directory"/data*; do
    echo "FILE: $file"
    cat "$file" >> "$output_file"
done

echo "All data files have been merged into $output_file"

