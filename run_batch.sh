#!/bin/bash


folder="data/intervals/M/small"

counter=$(find "data/results" -type f | wc -l)

# We hack the format to be a number, otherwise it adds a lot of spaces on the first element
((counter++))
((counter--))


optimisations=(-m -n -u -b
               -n\ -u -n\ -b)

 
find "$folder" -type f | while read -r file; do
    for opt in "${optimisations[@]}"; do
        echo "OPT: $opt"
        echo "./src/bin/BBCMMS -d $file -e data/results/data$counter.txt $opt"
        ./src/bin/BBCMMS -d "$file" -e "data/results/data$counter.txt" $opt
        ((counter++))
    done
done







