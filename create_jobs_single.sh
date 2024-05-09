#!/bin/bash

# Description: This script will take a file path to and instance and create a
# job for every single combination of optimisations from and including the one
# passed as an argument, if no argument is passed, then all will be included.


if [ $# -eq 0 ]; then
    echo "Usage: $0 <path/to/data/file> (optional)[optimisation]"
    exit 1
fi

data_path="$1"
data_name="${data_path//\//_}"

identifier=""
extra_argument=""

if [ ! -f "$data_path" ]; then
    echo "Could not find file: $data_path"
    exit 1
fi

last_opt=""
for arg in "${@:2}"; do
    if [ -n "$last_opt" ]; then
        last_opt+=" "  # Add a delimiter if the result is not empty
    fi
    last_opt+="$arg"
done

#last_opt=$(echo "$last_opt" | tr -d '\n' | tr -d '\r' | sed 's/[[:space:]]*$//')


optimisations=()

# Read the file line-by-line and add each line to the array
while IFS= read -r line; do
    optimisations+=("$line")
done < optimisations.txt

if [[ -n "$last_opt" ]]; then
    index=-1
    for i in "${!optimisations[@]}"; do
        if [[ "${optimisations[$i]}" == "$last_opt" ]]; then
            index=$i
            break
        fi
    done
   
    if [[ $index -ge 0 ]]; then
        # Extract all elements from the found index onward
        opts_to_run=("${optimisations[@]:$index}")
    else
        echo "Given opt '$last_opt' not found in the list."
        exit 1
    fi 
else
    opts_to_run=("${optimisations[@]}")
fi

counter=0

for opt in "${opts_to_run[@]}"; do
    opt_label=${opt//\ /_}
    opt_label=${opt_label//-/}
    run_plan="#!/bin/sh
#SBATCH --partition=CPUQ
#SBATCH --job-name=${data_name}_${opt_label}
#SBATCH --account=ie-idi
#SBATCH --time=0-01:00:00     # 0 days and 30 minutes limit
#SBATCH --nodes=1             # 1 compute nodes
#SBATCH --cpus-per-task=1     # 2 CPU cores
#SBATCH --mem=128M             # 32 megabytes memory
#SBATCH --output=output/log_${data_name}_${opt_label}.txt    # Log file
echo 'Single'
./src/bin/BBCMMS -d "$data_path" -e "data/results/data_${data_name}_${opt_label}.txt" $opt
echo 'Job finished'"

    echo "$run_plan" > "run_plans/job_${data_name}_${opt_label}.slurm"
    ((counter++))
done

echo "$counter"
