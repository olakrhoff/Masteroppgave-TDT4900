#!/bin/bash

#Description: This script is responsible for checking the state of the experiments
#if some are done cleanup is executed, if some failed/timed out then rescheduling 
#is done. It reports all its findings and a status of how many job(s) are running
#and how many was started now

username=$(cat username.txt)
jobs_running=$(squeue -u $username | grep "$username" | wc -l)
echo "$jobs_running job(s) are still running"


echo "Cleaning up finished jobs..."
finished_jobs=$(./remove_finished_jobs.sh)
echo "$finished_jobs new job(s) finished and cleaned up"

echo "Moving failed jobs..."
moved_failed_jobs=$(./move_failed_jobs.sh output timeout_files)
echo "$moved_failed_jobs moved"

failed_jobs=$(ls timeout_files | grep ".txt" | wc -l)
if [ "$failed_jobs" -eq 0 ]; then
    echo "No failed job(s) to handle, done"
    exit 0
fi

echo "Rescheduling intervals..."
intervals=0
for failed_file in timeout_files/*; do
    first_line=$(head -n 1 "$failed_file")
    if [[ "$first_line" != "Interval" ]]; then
        continue
    fi
    val=$(./reschedule_interval.sh $failed_file | tail -n 1)
    if [[ "$val" =~ ^-?[0-9]+$ ]]; then
        intervals=$((intervals + val))
    else
        echo "Error: The output from reschedule_interval.sh is not a valid number: '$val'"
        exit 1
    fi
done
echo "$intervals new job(s) from intervals created"


echo "Rescheduling instances..."
instances=0
for failed_file in timeout_files/*; do
    first_line=$(head -n 1 "$failed_file")
    if [[ "$first_line" != "Instance" ]]; then
        continue
    fi
    val=$(./reschedule_instance.sh $failed_file | tail -n 1)
    if [[ "$val" =~ ^-?[0-9]+$ ]]; then
        instances=$((instances + val))
    else
        echo "Error: The output from reschedule_instance.sh is not a valid number: '$val'"
    fi
done
echo "$instances new job(s) from instances created"

new_jobs=$((intervals + instances))
echo "Staring new the $new_jobs created..."
./start_all_jobs.sh
echo "All jobs started"


echo "Handling timedout single instance and configuration jobs..."
echo "TODO"
singles=0
echo "$singles cases handled"
