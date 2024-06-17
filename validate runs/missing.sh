#!/bin/bash

# Check if exactly three arguments are provided
if [[ $# -ne 3 ]]; then
  echo "Usage: $0 larger_file subset_file output_file"
  exit 1
fi

# Assign the arguments to variables
larger_file="$1"
subset_file="$2"
output_file="$3"

# Check if both input files exist
if [[ ! -f "$larger_file" ]]; then
  echo "Larger file '$larger_file' does not exist."
  exit 1
fi

if [[ ! -f "$subset_file" ]]; then
  echo "Subset file '$subset_file' does not exist."
  exit 1
fi

# Use grep to find rows in larger_file that are not in subset_file and write to output_file
grep -Fxv -f "$subset_file" "$larger_file" > "$output_file"

# Check if the operation was successful
if [[ $? -eq 0 ]]; then
  echo "Rows unique to $larger_file have been written to $output_file"
else
  echo "An error occurred."
  exit 1
fi

