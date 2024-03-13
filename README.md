# Masteroppgave-TDT4900

Run ```make <target>``` to execute the following steps. Running just ```make``` will default to ```make run```

### Targets

* setup: Creates the necessary directories (plots, data, and src/bin) to organize the project files.
* clean: Removes all compiled binaries in src/bin and any generated plots in plots.
* build: Compiles the C++ code using g++ with C++20 standard and O3, creating the executables in ```src/bin```.
* execute: Builds the project, (WILL) generates the dataset and runs the compiled executable. Then runs a Python script (```analysis.py```) for analyzing the generated data, which generates the plots in ```plots```.
* run: Cleans the project, sets it up, executes it.
* analysis: Runs the analysis part
* test: Runs tests to validate that parts of the code is working

## Dependencies
* LP Solver: ```brew install lp_solve```
* Scikit-learn: ```pip3 install scikit-learn```

## Running scripts
Here we will look at and document the different command line options for the different programs supporting arguments.

### BBCMMS.cpp

Usage: ```BBCMMS -d <input_filepath> -o <output_filepath> -x[from:to] <x_axis_data> -y <y_axis_data> [options]```

#### Input and output
* -d
  * This is the filepath to the data file.
  * Example: ```-d data/data_file.txt```
* -o
  * This is the filepath to the the output file, where the results are stored. Note that this is ```data/analysis``` if no argument is passed. This is so that the results gets placed in the data fodler and the filename is prefixed with analysis, since this is the "hook" the plotting script uses to find the files to plot. The program adds a lot more to the name based on which optimisations has been actived, and the attributes of the instance.
  * Example: ```-o data/analysis```
* -x[from:to]
  * g: GOODS
  * a: AGNETS
  * n: N_OVER_M
  * This argument is used if we want to run an interval over the x-axis for plotting purposes.
  * Example: ```-x g5:10```
* -y
  * t: TIME
  * n: NODES
  * This is the same as for the -x argument above, only for the y-axis for plotting.

#### Optimisations
* -u
  * This argument turn on the upper bound optimisation. In short a LP solver is ran on each instance and if the upper bound of that instance is not higher than our best solution thus far, we prune the subproblem.
* -b
  * This argument turns on the bound and bound optimisation. This requires an upper bound to work so it turns on the -u argument as well. In short this checks if the lower bound is higher than the best solution thus far, if so we update our best solution thus far. Also if the bounds are tight (equal) we conclude the search for the node (instance) and prune the rest since we have implicitly gotten the answer.
* -p
  * This is the argument for the picking order of the goods. If this argument is not passed, the RANDOM picking order is default.
  * r: RANDOM
  * n: NASH
  * v: MAX_VALUE
  * w: MAX_WEIGHT
  * p: MAX_PROFIT
* -r
  * This arguments reverses the order of the picking order
#### Other
* -e
  * This arguement makes it so that the results are written (exported) to the global results table. This is the table used for the generation of the decision tree.
 
### generate_dataset.cpp
Usage: ```generate_dataset -o <output_filepath> -a <from>:<to>  -g <from>:<to> -i g5```

* -a
  * This sets the number of agents
  * Example: ```-a 3:5```
* -g
  * This sets the number of goods
  * Example: ```-g 5:15```
* -o
  * This sets the output filepath. If we generate an interval of datasets, a counter will be added before the file type (i.e. ```filename<count>.txt```).
  * The program will create the directories in the filepath if it can't find them. If the file already exists, it will prompt the user with this info and the choice of either overwriting or aborting.
* -i
  * This sets which attribute we want to generate an interval for, it it is not passed, then we don't generate an interval. All that will happen is that the range which the respective attribute has been given (in its respective argument) will be the interval and not a range from which we randomly choose a value. After the attribute is choosen we expect a number for the number of splits in the interval.
  * a: AGENTS
  * g: GOODS
  * p: AVG_PERMUTATION_DISTANCE
  * v: AVG_VALUE_DISTANCE
  * r: M_OVER_N (ratio)
  * b: BUDGET_USED_PERCENT
  * Example: ```-i a5``` (use the agent interval and split it into 5 steps)
* -p
  * Average permutation distance.
* -v
  * Average value distance.
* -r
  * M over N ratio.
* -b
  * Budget used percent
  * Format: ```-b <from>:<to>``` the values are integers in for of percent
  * Example: ```-b 50:150``` gives the interval of 50% to 150%
  * This defaults to ```100:150``` if the argument is not passed, this means that we will scale the budgets by default so that there will almost guaranteed be a small or medium charity.
* -d
  * Distributions:
  * Format: ```-d <option><dist>...<option><dist>```
  * Takes a string of pairwise alternatives the options are
    * b: BUDGET
    * v: VALUE
    * w: WEIGHT
  * And the distributions are:
    * r: RANDOM
    * n: NORMAL
    * u: UNIFORM
  * Example: ```-d brwu```. Sets budget to random dist. and weights to uniform dist., value is not set, so it is random by default.
  * Note: if a later assignes a different value to the option the option will be overriden.
