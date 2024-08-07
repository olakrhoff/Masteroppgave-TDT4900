#!/bin/bash

#Description: This script is responsible for checking the state of the experiments
#if some are done cleanup is executed, if some failed/timed out then rescheduling 
#is done. It reports all its findings and a status of how many job(s) are running
#and how many was started now

username=$(cat username.txt)
total_jobs=$(squeue -u $username | grep "$username" | wc -l)
jobs_running=$(squeue -u $username | grep "$username  R" | wc -l)
jobs_queued=$((total_jobs - jobs_running))
echo "$jobs_running job(s) are running, $jobs_queued job(s) are queued, total job(s) left: $total_jobs"


echo "Cleaning up finished jobs..."
echo "----------------------------"
./remove_finished_jobs.sh
echo "----------------------------"

echo "Moving failed jobs..."
echo "----------------------------"
./move_failed_jobs.sh output timeout_files
echo "----------------------------"

failed_jobs=$(ls timeout_files | grep ".txt" | wc -l)
if [ "$failed_jobs" -eq 0 ]; then
    echo "No failed job(s) to handle, done"
else
    echo "Rescheduling intervals..."
    if [ ! -z "$(ls -A timeout_files)" ]; then
        intervals=0
        for failed_file in timeout_files/*; do
            first_line=$(head -n 1 "$failed_file")
            if [[ "$first_line" != "Interval" ]]; then
                continue
            fi
            val=$(./reschedule_interval.sh $failed_file | tail -n 1)
            if [[ "$val" =~ ^-?[0-9]+$ ]]; then
                intervals=$((intervals + val))
                echo -ne "\rFiles: $intervals"
            else
                echo "Error: The output from reschedule_interval.sh is not a valid number: '$val'"
                exit 1
            fi
        done
        echo
        echo "$intervals new job(s) from intervals created"
    else
        echo "No more timedout files to check, skipping interval checking"
    fi

    echo "Rescheduling instances..."
    instances=0
    if [ ! -z "$(ls -A timeout_files)" ]; then
        for failed_file in timeout_files/*; do
            first_line=$(head -n 1 "$failed_file")
            if [[ "$first_line" != "Instance" ]]; then
                continue
            fi
            val=$(./reschedule_instance.sh $failed_file | tail -n 1)
            if [[ "$val" =~ ^-?[0-9]+$ ]]; then
                instances=$((instances + val))
                echo -ne "\rFiles: $instances"
            else
                echo "Error: The output from reschedule_instance.sh is not a valid number: '$val'"
            fi
        done
        echo
        echo "$instances new job(s) from instances created"
    else
        echo "No instances to handle"
    fi


    echo "Handling timedout single instance and configuration jobs..."
    if [ ! -z "$(ls -A timeout_files)" ]; then
        singles=0

        all_files=(timeout_files/*)

        for file in "${all_files[@]}"; do
            type=$(cat "$file" | head -n 1)
            if [[ "$type" != "Single" ]]; then
                continue
            fi
            ./set_timeout.sh $file
            ((singles++))
            echo -ne "\rFiles: $singles"
        done
        echo
        echo "$singles cases handled"
    else
        echo "No singles to handle"
    fi
fi

max_jobs=20000
to_start_jobs=$((max_jobs - total_jobs))
local_counter=0
ready_jobs="$(ls -A run_plans | wc -l)"
if (( ready_jobs > 0)); then
    echo "Starting the $ready_jobs new job(s) created..."
    for job in run_plans/*; do
        if (( local_counter >= to_start_jobs )); then
            echo
            echo "Maximum number of jobs queued"
            break
        fi
        sbatch "$job" > /dev/null 
        rm "$job"
        ((local_counter++))
        percent=$((100 * local_counter / ready_jobs))

        echo -ne "\rProgress: $percent% ($local_counter of $ready_jobs files processed)"
    done
    echo
fi

if [ ! -z "$(ls -A data/results)" ]; then
    echo "Adding results to global table..."
    echo "---------------------------------"
    ./add_to_global.sh
    echo "---------------------------------"
    echo "Global table updated"
fi
