#!/bin/bash

# Description: This script takes a log file from a failed interval
# as input and creats jobs for all the instances that did not finish.
# After it has created the new jobs the file passed as the argument
# will be deleted to ensure that we don't generate duplicates later.

if [ $# -ne 1 ]; then
    echo "Usage: $0 <path/to/interval/log/file>"
    exit 1
fi

log_file="$1"

if [ ! -f "$log_file" ]; then
	echo "File given was not a file: $log_file"
	exit 1
fi

filtered_line=$(grep 'Starting branch and bound for batch:' $log_file | tail -n 1)

last_instance=$(echo "$filtered_line" | sed -n 's/.*\(data*\)/\1/p')

directory=$(echo "$last_instance" | sed -n 's/\(.*\)[/][^/]*/\1/p')

all_instances=$(ls $directory)

last_attempted_file=$(echo "$last_instance" | sed -n 's/.*[/]\(.*\)/\1/p')

# Get the index of the given name in the list
index=$(echo "$all_instances" | awk -v name="$last_attempted_file" '$0 == name {print NR}')

# Extract the part of the list that comes after the given name
if [ -n "$index" ]; then
    instances_to_run=($(echo "$all_instances" | awk -v idx="$index" 'NR >= idx'))
else
    echo "Given name not found in the list."
    exit 1
fi

counter=0
# Now we have the list of files that we need to create jobs for
for file in "${instances_to_run[@]}"; do
    echo "$file"
    ./create_jobs.sh "$directory/$file"
    ((counter++))
done

rm $log_file

echo "$counter"
