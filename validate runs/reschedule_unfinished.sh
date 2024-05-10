#!/bin/bash


file="to_run.txt"


declare -a ub=("" "-u")
declare -a bb=("" "-b")
declare -a non=("" "-n")
declare -a ord_g=("" "-p gr" "-p gn" "-p gv" "-p gw" "-p gp")
declare -a ord_a=("" "-p ar" "-p an" "-p av" "-p aw" "-p ap")
declare -a rev_g=("" "-r g")
declare -a rev_a=("" "-r a")
declare -a mip=("" "-m")

counter=0
total_files=$(cat "$file" | wc -l)

while IFS= read -r line; do

    filename=$(echo "$line" | sed -n "s/\(.*txt\).*/\1/p")
    data_name="${filename//\//_}"
    opts=$(echo "$line" | sed -n "s/.*,\([0-1],[0-1],[0-1],[0-5],[0-5],[0-1],[0-1],[0-1]\),[0-9]*$/\1/p")

    if [ -n "$opts" ]; then
        # Create an array by splitting the string on commas
        IFS=',' read -ra opt_array <<< "$opts"

        # Convert each element to an integer (if needed)
        # This is more for demonstration; Bash variables are inherently strings
        for i in "${!opt_array[@]}"; do
            opt_array[$i]=$((opt_array[$i]))  # Convert to integer
        done

        # Display the array to confirm the values
    else
        echo "No opts found in the line."
        exit 1
    fi

    opt="${ub[${opt_array[0]}]} ${bb[${opt_array[1]}]} ${non[${opt_array[2]}]} ${ord_g[${opt_array[3]}]} ${ord_a[${opt_array[4]}]} ${rev_g[${opt_array[5]}]} ${rev_a[${opt_array[6]}]} ${mip[${opt_array[7]}]}"

    opt=$(echo "$opt" | tr -s " ")

    run_plan="#!/bin/sh
#SBATCH --partition=CPUQ
#SBATCH --job-name=${data_name}_${opt//\ /_}
#SBATCH --account=ie-idi
#SBATCH --time=0-01:00:00     # 0 days and 60 minutes limit
#SBATCH --nodes=1             # 1 compute nodes
#SBATCH --cpus-per-task=1     # 2 CPU cores
#SBATCH --mem=128M             # 32 megabytes memory
#SBATCH --output=output/log_${data_name}_${opt//\ /_}.txt    # Log file
echo 'Single'
./src/bin/BBCMMS -d "$filename" -e "data/results/data_${data_name}_${opt//\ /_}.txt" $opt

echo 'Job finished'"

    echo "$run_plan" > "run_plans/job_${data_name}_${opt//\ /_}.slurm"

    ((counter++))
    percent=$((100 * counter / total_files))

    echo -ne "\rProgress: $percent% ($counter of $total_files files processed)"

done < "$file"
echo
