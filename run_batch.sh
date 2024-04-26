#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path/to/data>"
    exit 1
fi

folder="$1"
folder_name="${folder//\//_}"


if [ ! -f "$folder" ] && [ ! -d "$folder" ]; then
    echo "Path given is not to a file nor a directory"
    exit 1
fi

counter=$(find "data/results" -type f | wc -l)

opt_filepath="optimisations.txt"

optimistaions=()

while IFS= read -r line; do
    optimisations+=("$line")
done < "$opt_filepath"

find "$folder" -type f | while read -r file; do
    for opt in "${optimisations[@]}"; do
        echo "OPT: $opt"
        echo "./src/bin/BBCMMS -d $file -e data/results/data_${folder_name}_${opt//\ /_}.txt $opt"
        ./src/bin/BBCMMS -d "$file" -e "data/results/data_${folder_name}_${opt//\ /_}.txt" $opt
        ((counter++))
    done
done







