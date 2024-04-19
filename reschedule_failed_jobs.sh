#!/bin/bash

#Description: This script will look at the all failed jobs.
#If the job was run on an interval level, then all elements
#in the interval will be run in seperate jobs.
#If the job was run on indiviual instances, then all
#configurations will be run on the instance in seperate jobs.
#If the job was run on a single config and instance, we rerun
#it with a simple instance, then change the time in the data
#generated to the TIMEOUT time.


# First we loop over all the log files for the failed jobs
for file in timeout_files/*.txt; do
	if [ -f "$file" ] && [ -r "$file" ]; then
		echo $file
		if head -n 1 "$file" | grep -q "Interval"; then
			echo "Log failed interval"
			./reschedule_interval.sh $file
		elif head -n 1 "$file" | grep -q "Instance"; then
			echo "Log failed on instance"
			./reschedule_instance.sh $file
		else
			echo "Log failed on single run"
			./calibrate_max_time.sh $file
		fi
	fi
done
