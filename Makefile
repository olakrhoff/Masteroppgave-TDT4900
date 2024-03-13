CC = g++
CFLAGS = -std=c++20 -O3 -g -Wall -Wextra
LDFLAGS = -llpsolve55
INCLUDE_DIRS = -I$(CURDIR)/include/

SRC_FILES = $(CURDIR)/src/BBCMMS.cpp $(CURDIR)/src/LPSolver.cpp 
OBJ_FILES = $(SRC_FILES:.cpp=.o)


CURDIR = '$(PWD)'
.PHONY: setup run clean execute build analysis run_bb tree test fix gen

run: setup clean execute

setup:
	mkdir -p plots data src/bin

clean:  
	rm -rf $(CURDIR)/src/bin/*
	rm test test.pdf decision_tree

build:
	$(CC) $(CFLAGS) $(CURDIR)/src/generate_dataset.cpp -o $(CURDIR)/src/bin/generate_dataset $(INCLUDE_DIRS)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(CURDIR)/src/bin/BBCMMS $(LDFLAGS) $(INCLUDE_DIRS)
	$(CC) $(CFLAGS) $(CURDIR)/src/test_LPSolver.cpp $(CURDIR)/src/LPSolver.cpp -o $(CURDIR)/src/bin/test_LPSolver $(LDFLAGS) $(INCLUDE_DIRS)

# Need to be run to update the include paths such that the LSP reconises files
fix:
	bear -- make build

sim: build
	./src/bin/generate_dataset -a 3 -g 10 -o data/testing_shit.txt
	./src/bin/BBCMMS -d data/testing_shit.txt -o data/analysis -x g1:15 -u -b
	python3 src/analysis.py 

gen: build
#	./src/bin/generate_dataset -a 3:15 -g 6:6 -o data/intervals/N/small.txt -i a5
	./src/bin/generate_dataset -a 3:3 -g 5:20 -d wn -o data/intervals/M/small.txt -i g5

execute: build
		./src/bin/generate_dataset # Generate the dataset
		./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis -x g1:12 -u
		python3 src/analysis.py # Run the analysis of the generated data

run_bb: clean setup build
	./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:12 -u
	./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:12 -u -b

analysis:
	python3 src/analysis.py

tree:
	python3 src/decision_tree.py data/decision_tree.txt

test: setup build
	./src/bin/test_LPSolver
	./src/bin/BBCMMS -d data/negative_proof_39_40.txt -o data/analysis -x g9:9 -u -b -e
	python3 src/decision_tree.py test
