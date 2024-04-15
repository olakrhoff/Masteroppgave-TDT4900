#!/bin/zsh


folder="data/intervals/M/small"

counter=$(find "data/results" -type f | wc -l)

# We hack the format to be a number, otherwise it adds a lot of spaces on the first element
((counter++))
((counter--))
 
find "$folder" -type f | while read -r file; do
    ./src/bin/BBCMMS -d "$file" -e "data/results/data$counter.txt" -m
    ((counter++))
done







