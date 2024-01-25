CC = g++
CFLAGS = -std=c++20 -O3 -g

CURDIR = '$(PWD)'
.PHONY: setup run clean execute build analysis run_bb

run: setup clean execute

setup:
		mkdir -p plots data src/bin

clean:  
		rm -rf $(CURDIR)/src/bin/*
		rm -rf $(CURDIR)/plots/*

build:
		$(CC) $(CFLAGS) $(CURDIR)/src/generate_dataset.cpp -o $(CURDIR)/src/bin/generate_dataset
		$(CC) $(CFLAGS) $(CURDIR)/src/BBCMMS.cpp -o $(CURDIR)/src/bin/BBCMMS

execute: build
		./src/bin/generate_dataset # Generate the dataset
		./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:15
		python3 src/analysis.py # Run the analysis of the generated data

run_bb: clean setup build
	./src/bin/BBCMMS -d data/input_data_test.txt -o data/analysis_naive -x g1:15

analysis:
		python3 src/analysis.py
