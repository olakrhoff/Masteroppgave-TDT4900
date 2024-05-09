#!/bin/bash

# Description: This script takes a directory path or a file path and 
# creates a job for each interval (directory containing files) or 
# the explicit file. Or it can split a single file into multiple jobs.
# The second arguement controls this. It is 'i', if it is passed as
# a second arguemnt and every single file in the directory or just the
# file given will have a job created for each combination of optimisation
# configuration for the respective instance.

if [ $# -ne 1 ] && [ $# -ne 2 ]; then
    echo "Usage: $0 <path/to/data> (optional)[(i)ndividual]"
    exit 1
fi

data_path="$1"
data_name="${data_path//\//_}"

type=""
identifier=""
extra_argument=""

if [ -d "$data_path" ]; then
    type="dir"
    identifier="Interval"
fi

if [ -f "$data_path" ]; then
    type="file"
    identifier="Instance"
fi

if [ $# -eq 2 ]; then
    second_arg="$2"
    if [[ ! "$second_arg" == "i" ]]; then
        echo "Expected second argument to be: 'i'"
        echo "Got '$second_arg' instead"
        exit 1
    fi
    identifier="Single"
    extra_argument="i"
fi

if [[ "$identifier" == "Single" ]]; then
    if [[ "$type" == "dir" ]]; then
       for file in "$data_path"/*; do
           ./create_jobs_single.sh $file
       done
    else # file
        ./create_jobs_single.sh $data_path
    fi
else
    run_plan="#!/bin/sh
#SBATCH --partition=CPUQ
#SBATCH --job-name=${data_name}
#SBATCH --account=ie-idi
#SBATCH --time=0-00:01:00     # 0 days and 30 minutes limit
#SBATCH --nodes=1             # 1 compute nodes
#SBATCH --cpus-per-task=1     # 2 CPU cores
#SBATCH --mem=128M             # 32 megabytes memory
#SBATCH --output=output/log_${data_name}.txt    # Log file
echo '$identifier'
./run_batch.sh ${data_path}
echo 'Job finished'"

    echo "$run_plan" > "run_plans/job_${data_name}.slurm"
fi

echo "Run plan(s) created for patch: $data_path"
