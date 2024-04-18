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
		else
			echo "Log failed something else"
		fi
	fi
done
