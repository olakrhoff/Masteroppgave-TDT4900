#!/bin/bash

#Description: This script starts all the jobs
#that are located in the run_plans directory.
#After starting the job the file is deleted.

directory="run_plans"

for file in "$directory"/*; do
    if [ -f "$file" ]; then
	sbatch $file
	rm $file
    fi
done
