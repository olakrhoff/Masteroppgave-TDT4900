# Masteroppgave-TDT4900

Run ```make <target>``` to execute the following steps. Running just ```make``` will default to ```make run```

### Targets

* setup: Creates the necessary directories (plots, data, and src/bin) to organize the project files.
* clean: Removes all compiled binaries in src/bin and any generated plots in plots.
* build: Compiles the C++ code using g++ with C++20 standard and O3, creating the executable in ```src/bin``` as ```main```.
* execute: Builds the project, (WILL) generates the dataset and runs the compiled executable. Then runs a Python script (```analysis.py```) for analyzing the generated data, which generates the plots in ```plots```.
* run: Cleans the project, sets it up, executes it.
* analysis: Runs the analysis part
* test: Runs tests to validate that parts of the code is working

## Dependencies
* LP Solver: ```brew install lp_solve```
* Scikit-learn: ```pip3 install scikit-learn```
