#!/bin/bash

echo "Finding premeture files"
for file in output/*; do
    last_line=$(tail -n 1 "$file")
    if [[ "$last_line" == "Job finished" ]]; then
       echo "$file"
    fi
done

echo "Finding failed files"


known_values=()
readarray -t known_values < optimisations.txt

counter=1

# Iterate over the files in the directory
for file in output/*; do

    opt=$(echo "$file" | sed -n "s/.*medium2.txt\(.*\).txt/\1/p" | sed -n "s/_/ -/g;p" | sed -n "s/-p -\([a-z][a-z]\)/-p \1/g;p" | sed -n "s/-r -\([a-z]\)/-r \1/g;p" | sed -n "s/ \(.*\)/\1/p")
    
    for val in "${known_values[@]}"; do
        echo "$val"
        if [[ "$val" == "$opt" ]]; then 
            new_array=()
            for element in "${known_values[@]}"; do
                if [[ "$element" != "$val" ]]; then
                    new_array+=("$element")
                fi
            done

            known_values=("${new_array[@]}")
            echo "${#known_values[@]}"
            break
        fi
    done
    #echo -ne "\033[2K\r"
    #echo -ne "\rFile #$counter: $opt"
    
    #read -n 1 -s  # -s suppresses output to keep keypress invisible
    # Increment the counter
    #((counter++))
done


for val in "${known_values[@]}"; do
    echo "$counter: $val"
    ((counter++))
done
