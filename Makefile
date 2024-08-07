ARCH := $(shell uname -m)
OS := $(shell uname -s)


ifeq ($(OS), Linux)
	LP_SOLVE_PATH = lib/lp_solve_5.5/lpsolve55/bin/ux64
	LIBRARY_PATH = -L$(LP_SOLVE_PATH)
	INCLUDE_PATH = -Ilib/lp_solve_5.5
	#export LD_LIBRARY_PATH := $(LP_SOLVE_PATH):$(LD_LIBRARY_PATH) #This is needed if we dynamically link it, but that does not work when queueing jobs
else ifeq ($(OS), Darwin)
	ifeq ($(ARCH), arm64)
		LIBRARY_PATH = -L/opt/homebrew/lib
		INCLUDE_PATH = -I/opt/homebrew/include
	else
		LIBRARY_PATH =
		INCLUDE_PATH =
	endif
else # Not handled
	$(error Operating system ($(OS)) not supported.)
endif



CC = g++
CFLAGS = -std=c++20 -O3 -g -Wall -Wextra $(INCLUDE_PATH)
LDFLAGS = $(LIBRARY_PATH) -llpsolve55
INCLUDE_DIRS = -I$(CURDIR)/include/

SRC_FILES = $(CURDIR)/src/BBCMMS.cpp $(CURDIR)/src/LPSolver.cpp 
OBJ_FILES = $(SRC_FILES:.cpp=.o)


CURDIR = '$(PWD)'

.PHONY: setup run clean execute build analysis run_bb tree test fix gen format_data paths start_experiments manage_experiments clear

run: setup clean execute

setup:
	mkdir -p plots data data/results src/bin run_plans

clean:  
	rm -rf $(CURDIR)/src/bin/*

clear:
	rm -f data/results/* output/* run_plans/* timeout_files/*

build:  paths
	$(CC) $(CFLAGS) $(CURDIR)/src/generate_dataset.cpp -o $(CURDIR)/src/bin/generate_dataset $(INCLUDE_DIRS)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(CURDIR)/src/bin/BBCMMS $(LDFLAGS) $(INCLUDE_DIRS)
	$(CC) $(CFLAGS) $(CURDIR)/src/test_LPSolver.cpp $(CURDIR)/src/LPSolver.cpp -o $(CURDIR)/src/bin/test_LPSolver $(LDFLAGS) $(INCLUDE_DIRS)

# Need to be run to update the include paths such that the LSP recognises files
fix:
	bear -- make build

sim: build
	./src/bin/generate_dataset -a 3 -g 10 -o data/testing_stuff.txt
	./src/bin/BBCMMS -d data/testing_stuff.txt -o data/analysis -x g1:15 -u -b
	python3 src/analysis.py 

gen: build
	#./src/bin/generate_dataset -a 5:5 -g 6:6 -d wnbn -v 0:100 -o data/intervals/values/small.txt -i v5
	#src/bin/generate_dataset -o data/testing.txt   -a 2:4 -r 1:3 -b 50:250 -i b5
	./generate_data_script.sh


execute: build
		./src/bin/generate_dataset # Generate the dataset
		./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis -x g1:12 -u
		python3 src/analysis.py # Run the analysis of the generated data

run_bb: build
#	./src/bin/BBCMMS -d data/intervals/N/small2.txt -o data/analysis_small -x g24:24 -u
#	./src/bin/BBCMMS -d data/intervals/M/small/bnvnwn/small2.txt -o data/analysis_small -e data/results/data1.txt -u -b -n -p gn -p ap
	./src/bin/BBCMMS -d data/intervals/N/medium/bnvnwn/medium1.txt -e data/results/data1.txt -m
	./src/bin/BBCMMS -d data/intervals/N/medium/bnvnwn/medium1.txt -e data/results/data2.txt -n -u -p gp -r g -p ap
	./src/bin/BBCMMS -d data/intervals/N/medium/bnvnwn/medium1.txt -e data/results/data3.txt

analysis:
	python3 src/analysis.py

tree:
	python3 src/decision_tree.py data/decision_tree.txt

test: setup build
	./src/bin/test_LPSolver
	./src/bin/BBCMMS -d data/negative_proof_39_40.txt -o data/analysis -x g9:9 -u -b -e
	python3 src/decision_tree.py test

format_data: 
	sed 's/,/,:/g' data/global_results.txt | column -t -s: | sed 's/ ,/,/g' > data/global_results_formated.txt

start_experiments:
	./start_experiments.sh

manage_experiments:
	./manage_experiments.sh
