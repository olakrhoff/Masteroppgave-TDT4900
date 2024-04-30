#!/bin/bash


file="global_results_batch_1.csv"

> "corrected.csv"
first=true
while IFS= read -r line; do
    if $first; then
        echo "$line" >> "corrected.csv"
        first=false
        continue
    fi
    vals=$(echo "$line" | sed -n "s/.*,.*,\(.*\),.*,\(.*\),.*,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*/\1,\2/p")
    perm=$(echo "$vals" | sed -n "s/.*,//p")
    #m=$(echo "$line" | sed -n "s/.*,.*,\(.*\),.*,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*/\1/p")
    m=$(echo "$vals" | sed -n "s/,.*//p")
    max_perm=$(echo "0.5*$m*($m-1)" | bc)
    new_val=$(echo "$perm / $max_perm" | bc -l | sed -n "s/./0./p")
    new_line=$(echo "$line" | sed -n "s/\(.*,.*,.*,.*,\).*\(,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*,.*\)/\1 $new_val\2/p")
    echo "$new_line" >> "corrected.csv"
done < "$file" 
