#!/bin/zsh


folder="data/intervals/M/small"

counter=$(find "data/results" -type f | wc -l)
 
find "$folder" -type f | while read -r file; do
    echo "$counter, FILE: $file"
    ((counter++))
    ./src/bin/BBCMMS -d "$file" -e "data/results/data$counter.txt" -m
done







