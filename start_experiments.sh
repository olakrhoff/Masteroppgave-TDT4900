#!/bin/bash

#Description: This script starts all the experiments

directories=(output timeout_files run_plans data/results)
for dir in "${directories[@]}"; do
    if [ ! -z "$(find "$dir" -mindepth 1 -maxdepth 1 -type f -o -type d)" ]; then
        echo "Directory '$dir' is not empty, starting experiments failed"
        exit 1
    fi
done

echo "Creating jobs..."
./construct_batches.sh data/intervals
echo "Starting jobs..."
./start_all_jobs.sh
echo "Experiments started"
