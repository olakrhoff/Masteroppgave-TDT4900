#!/bin/bash

#Description: This script is responsible for checking the state of the experiments
#if some are done cleanup is executed, if some failed/timed out then rescheduling 
#is done. It reports all its findings and a status of how many jobs are running
#and how many was started now

username=$(cat username.txt)
jobs_running=$(squeue -u $username | grep "$username" | wc -l)
echo "$jobs_running job(s) are still running"


echo "Cleaning up finished jobs..."
finished_jobs=$(./remove_finished_jobs.sh)
echo "$finished_jobs new jobs finished and clean up"

echo "Moving failed jobs..."
moved_failed_jobs=$(./move_failed_jobs.sh output timeout_files)
echo "$moved_failed_jobs moved"

failed_jobs=$(ls timeout_files | grep ".txt" | wc -l)
if [ "$failed_jobs" -eq 0 ]; then
    echo "No failed jobs to handle, done"
    exit 0
fi

echo "Rescheduling intervals..."
intervals=0
for failed_file in timeout_files/*; do
    val=$(./reschedule_interval.sh $failed_file)
    intervals=$((intervals + val))
done

echo "$intervals new jobs from intervals created"


echo "Rescheduling instances..."
echo "TODO"
instances=0
echo "$instances new jobs from instances created"

new_jobs=$((intervals + instances))
echo "Staring new the $new_jobs created..."
echo "TODO"
echo "All jobs started"


echo "Handling timedout single instance and configuration jobs..."
echo "TODO"
singles=0
echo "$singles cases handled"
