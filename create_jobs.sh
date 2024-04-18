#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <path/to/data>"
    exit 1
fi

data_path="$1"
data_name="${data_path//\//_}"


if [ ! -d "$data_path" ]; then
    echo "Directory not found: $data_path"
    exit 1
fi

run_plan="#!/bin/sh
#SBATCH --partition=CPUQ
#SBATCH --account=ie-idi
#SBATCH --time=0-00:15:00     # 0 days and 15 minutes limit
#SBATCH --nodes=1             # 1 compute nodes
#SBATCH --cpus-per-task=1     # 2 CPU cores
#SBATCH --mem=5G              # 5 gigabytes memory
#SBATCH --output=output/log_${data_name}.txt    # Log file
echo 'Starting job'
./src/bin/BBCMMS -d data/intervals/M/small/bnvnwn/small2.txt -e data/results/data1.txt -m
echo 'Job finsihed'"




echo "$run_plan" > "run_plans/job_${data_name}.slurm"
echo "Run plan created for patch: $data_path"

