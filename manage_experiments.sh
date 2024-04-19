#!/bin/bash

#Description: This script is responsible for checking the state of the experiments
#if some are done cleanup is executed, if some failed/timed out then rescheduling 
#is done. It reports all its findings and a status of how many jobs are running
#and how many was started now

echo "Cleaning up finished jobs..."
finished_jobs=$(./remove_finished_jobs.sh)
echo "$finished_jobs new jobs finished and clean up"




echo "TODO: Implement script"
