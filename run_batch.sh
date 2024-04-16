#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <path/to/data>"
    exit 1
fi

folder="$1"
folder_name="${folder//\//_}"

if [ ! -d "$folder" ]; then
    echo "Directory not found: $folder"
    exit 1
fi

counter=$(find "data/results" -type f | wc -l)

# We hack the format to be a number, otherwise it adds a lot of spaces on the first element
((counter++))
((counter--))

get_filename()
{
    timestamp=$(date +"%Y%m%d%H%M%S")
    random_number=$((RANDOM % 10000))
    unique_number="${timestamp}_${random_number}"
    filename="data_${unique_number}.txt"

    echo "$filename"
}
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







