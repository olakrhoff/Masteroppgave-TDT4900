#!/bin/bash


#file="jobs_to_delete_and_reschedule.txt"
file="delete_rerun.txt"

while IFS= read -r line; do
    ./create_jobs.sh "$line"
done < "$file"
