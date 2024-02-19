CC = g++
CFLAGS = -std=c++20 -O3 -g -Wall -Wextra
LDFLAGS = -llpsolve55
INCLUDE_DIRS = -I$(CURDIR)/src/LPSolver.h
SRC_FILES = $(CURDIR)/src/BBCMMS.cpp $(CURDIR)/src/LPSolver.cpp 
OBJ_FILES = $(SRC_FILES:.cpp=.o)


CURDIR = '$(PWD)'
.PHONY: setup run clean execute build analysis run_bb test

run: setup clean execute

setup:
	mkdir -p plots data src/bin

clean:  
	rm -rf $(CURDIR)/src/bin/*
	#rm -rf $(CURDIR)/plots/*

build:
	$(CC) $(CFLAGS) $(CURDIR)/src/generate_dataset.cpp -o $(CURDIR)/src/bin/generate_dataset
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(CURDIR)/src/bin/BBCMMS $(LDFLAGS) $(INCLUDE_DIRS)
	$(CC) $(CFLAGS) $(CURDIR)/src/test_LPSolver.cpp $(CURDIR)/src/LPSolver.cpp -o $(CURDIR)/src/bin/test_LPSolver $(LDFLAGS) $(INCLUDE_DIRS)

execute: build
		./src/bin/generate_dataset # Generate the dataset
		./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis -x g1:12 -u
		python3 src/analysis.py # Run the analysis of the generated data

run_bb: clean setup build
	./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:12 -u
	./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:12 -u -b

analysis:
	python3 src/analysis.py

test: setup build
	./src/bin/test_LPSolver
	./src/bin/BBCMMS -d data/negative_proof_39_40.txt -o data/analysis -x g9:9 -u -b
