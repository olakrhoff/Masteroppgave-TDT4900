#!/bin/bash


for opt in "${optimisations[@]}"; do
	echo "OPT: $opt"
	echo "./src/bin/BBCMMS -d $file -e data/results/data_${folder_name}_${opt//\ /_}.txt $opt"
	./src/bin/BBCMMS -d "$file" -e "data/results/data_${folder_name}_${opt//\ /_}.txt" $opt
done
