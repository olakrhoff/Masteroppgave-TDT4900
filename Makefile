CC = g++
CFLAGS = -std=c++20 -O3

CURDIR = '$(PWD)'
.PHONY: setup teardown run clean execute build reset analysis run_bb

setup:
		mkdir -p plots data src/bin

clean:  
		rm -f $(CURDIR)/src/bin/*
		rm -f $(CURDIR)/plots/*

build:
		$(CC) $(CFLAGS) $(CURDIR)/src/generate_dataset.cpp -o $(CURDIR)/src/bin/generate_dataset
		$(CC) $(CFLAGS) $(CURDIR)/src/BBCMMS.cpp -o $(CURDIR)/src/bin/BBCMMS

execute: build
		./src/bin/generate_dataset # Generate the dataset
		./src/bin/BBCMMS # Run the code
		python3 src/analysis.py # Run the analysis of the generated data

run: clean setup execute teardown

run_bb: clean setup build
		./src/bin/BBCMMS data/input_data_test.txt


analysis:
		python3 src/analysis.py

teardown:
		#rm -f $(CURDIR)/data/*

reset: 
		rm -rf $(CURDIR)/plots
		rm -rf $(CURDIR)/data
		rm -rf $(CURDIR)/src/bin
